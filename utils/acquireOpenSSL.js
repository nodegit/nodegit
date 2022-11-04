const crypto = require("crypto");
const execPromise = require("./execPromise");
// for fs.remove. replace with fs.rm after dropping v12 support
const fse = require("fs-extra");
const fsNonPromise = require("fs");
const { promises: fs } = fsNonPromise;
const path = require("path");
const got = require("got");
const { performance } = require("perf_hooks");
const { promisify } = require("util");
const stream = require("stream");
const tar = require("tar-fs");
const zlib = require("zlib");

const pipeline = promisify(stream.pipeline);

const packageJson = require('../package.json')

const OPENSSL_VERSION = "1.1.1s";
const win32BatPath = path.join(__dirname, "build-openssl.bat");
const vendorPath = path.resolve(__dirname, "..", "vendor");
const opensslPatchPath = path.join(vendorPath, "patches", "openssl");
const extractPath = path.join(vendorPath, "openssl");

const pathsToIncludeForPackage = [
  "include", "lib"
];

const getOpenSSLSourceUrl = (version) => `https://www.openssl.org/source/openssl-${version}.tar.gz`;
const getOpenSSLSourceSha256Url = (version) => `${getOpenSSLSourceUrl(version)}.sha256`;

class HashVerify extends stream.Transform {
  constructor(algorithm, onFinal) {
    super();
    this.onFinal = onFinal;
    this.hash = crypto.createHash(algorithm);
  }

  _transform(chunk, encoding, callback) {
    this.hash.update(chunk, encoding);
    callback(null, chunk);
  }

  _final(callback) {
    const digest = this.hash.digest("hex");
    const onFinalResult = this.onFinal(digest);
    callback(onFinalResult);
  }
}

const makeHashVerifyOnFinal = (expected) => (digest) => {
  const digestOk = digest === expected;
  return digestOk
    ? null
    : new Error(`Digest not OK: ${digest} !== ${this.expected}`);
};

// currently this only needs to be done on linux
const applyOpenSSLPatches = async (buildCwd, operatingSystem) => {
  try {
    for (const patchFilename of await fse.readdir(opensslPatchPath)) {
      const patchTarget = patchFilename.split("-")[1];
      if (patchFilename.split(".").pop() === "patch" && (patchTarget === operatingSystem || patchTarget === "all")) {
        console.log(`applying ${patchFilename}`);
        await execPromise(`patch -up0 -i ${path.join(opensslPatchPath, patchFilename)}`, {
          cwd: buildCwd
        }, { pipeOutput: true });
      }
    }
  } catch(e) {
    console.log("Patch application failed: ", e);
    throw e;
  }
}

const buildDarwin = async (buildCwd, macOsDeploymentTarget) => {
  if (!macOsDeploymentTarget) {
    throw new Error("Expected macOsDeploymentTarget to be specified");
  }

  const arguments = [
    process.arch === "x64" ? "darwin64-x86_64-cc" : "darwin64-arm64-cc",
    // speed up ecdh on little-endian platforms with 128bit int support
    "enable-ec_nistp_64_gcc_128",
    // compile static libraries
    "no-shared",
    // disable ssl2, ssl3, and compression
    "no-ssl2",
    "no-ssl3",
    "no-comp",
    // set install directory
    `--prefix="${extractPath}"`,
    `--openssldir="${extractPath}"`,
    // set macos version requirement
    `-mmacosx-version-min=${macOsDeploymentTarget}`
  ];

  await execPromise(`./Configure ${arguments.join(" ")}`, {
    cwd: buildCwd
  }, { pipeOutput: true });

  await applyOpenSSLPatches(buildCwd, "darwin");

  // only build the libraries, not the tests/fuzzer or apps
  await execPromise("make build_libs", {
    cwd: buildCwd
  }, { pipeOutput: true });

  await execPromise("make test", {
    cwd: buildCwd
  }, { pipeOutput: true });

  await execPromise("make install_sw", {
    cwd: buildCwd,
    maxBuffer: 10 * 1024 * 1024 // we should really just use spawn
  }, { pipeOutput: true });
};

const buildLinux = async (buildCwd) => {
  const arguments = [
    "linux-x86_64",
    // Electron(at least on centos7) imports the libcups library at runtime, which has a
    // dependency on the system libssl/libcrypto which causes symbol conflicts and segfaults.
    // To fix this we need to hide all the openssl symbols to prevent them from being overridden
    // by the runtime linker.
    "-fvisibility=hidden",
    // compile static libraries
    "no-shared",
    // disable ssl2, ssl3, and compression
    "no-ssl2",
    "no-ssl3",
    "no-comp",
    // set install directory
    `--prefix="${extractPath}"`,
    `--openssldir="${extractPath}"`
  ];
  await execPromise(`./Configure ${arguments.join(" ")}`, {
    cwd: buildCwd
  }, { pipeOutput: true });

  await applyOpenSSLPatches(buildCwd, "linux");

  // only build the libraries, not the tests/fuzzer or apps
  await execPromise("make build_libs", {
    cwd: buildCwd
  }, { pipeOutput: true });

  await execPromise("make test", {
    cwd: buildCwd
  }, { pipeOutput: true });

  // only install software, not the docs
  await execPromise("make install_sw", {
    cwd: buildCwd,
    maxBuffer: 10 * 1024 * 1024 // we should really just use spawn
  }, { pipeOutput: true });
};

const buildWin32 = async (buildCwd, vsBuildArch) => {
  if (!vsBuildArch) {
    throw new Error("Expected vsBuildArch to be specified");
  }

  const programFilesPath = (process.arch === "x64"
    ? process.env["ProgramFiles(x86)"]
    : process.env.ProgramFiles) || "C:\\Program Files";
  const vcvarsallPath = process.env.npm_config_vcvarsall_path || `${
    programFilesPath
  }\\Microsoft Visual Studio\\2017\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat`;
  try {
    await fs.stat(vcvarsallPath);
  } catch {
    throw new Error(`vcvarsall.bat not found at ${vcvarsallPath}`);
  }

  let vcTarget;
  switch (vsBuildArch) {
    case "x64": {
      vcTarget = "VC-WIN64A";
      break;
    }

    case "x86": {
      vcTarget = "VC-WIN32";
      break;
    }
      
    default: {
      throw new Error(`Unknown vsBuildArch: ${vsBuildArch}`);
    }
  }

  await execPromise(`"${win32BatPath}" "${vcvarsallPath}" ${vsBuildArch} ${vcTarget}`, {
    cwd: buildCwd,
    maxBuffer: 10 * 1024 * 1024 // we should really just use spawn
  }, { pipeOutput: true });
};

const removeOpenSSLIfOudated = async (openSSLVersion) => {
  try {
    let openSSLResult;
    try {
      const openSSLPath = path.join(extractPath, "bin", "openssl");
      openSSLResult = await execPromise(`${openSSLPath} version`);
    } catch {
      /* if we fail to get the version, assume removal not required */
    }

    if (!openSSLResult) {
      return;
    }

    const versionMatch = openSSLResult.match(/^OpenSSL (\d\.\d\.\d[a-z]*)/);
    const installedVersion = versionMatch && versionMatch[1];
    if (!installedVersion || installedVersion === openSSLVersion) {
      return;
    }

    console.log("Removing outdated OpenSSL at: ", extractPath);
    await fse.remove(extractPath);
    console.log("Outdated OpenSSL removed.");
  } catch (err) {
    console.log("Remove outdated OpenSSL failed: ", err);
  }
};

const makeOnStreamDownloadProgress = () => {
  let lastReport = performance.now();
  return ({ percent, transferred, total }) => {
    const currentTime = performance.now();
    if (currentTime - lastReport > 1 * 1000) {
      lastReport = currentTime;
      console.log(`progress: ${transferred}/${total} (${(percent * 100).toFixed(2)}%)`)
    }
  };
};

const buildOpenSSLIfNecessary = async ({
  macOsDeploymentTarget,
  openSSLVersion,
  vsBuildArch
}) => {
  if (process.platform !== "darwin" && process.platform !== "win32" && process.platform !== "linux") {
    console.log(`Skipping OpenSSL build, not required on ${process.platform}`);
    return;
  }

  if (process.platform === "linux" && process.env.NODEGIT_OPENSSL_STATIC_LINK !== "1") {
    console.log(`Skipping OpenSSL build, NODEGIT_OPENSSL_STATIC_LINK !== 1`);
    return;
  }

  await removeOpenSSLIfOudated(openSSLVersion);

  try {
    await fs.stat(extractPath);
    console.log("Skipping OpenSSL build, dir exists");
    return;
  } catch {}

  const openSSLUrl = getOpenSSLSourceUrl(openSSLVersion);
  const openSSLSha256Url = getOpenSSLSourceSha256Url(openSSLVersion);

  const openSSLSha256 = (await got(openSSLSha256Url)).body.trim();

  const downloadStream = got.stream(openSSLUrl);
  downloadStream.on("downloadProgress", makeOnStreamDownloadProgress());

  await pipeline(
    downloadStream,
    new HashVerify("sha256", makeHashVerifyOnFinal(openSSLSha256)),
    zlib.createGunzip(),
    tar.extract(extractPath)
  );

  console.log(`OpenSSL ${openSSLVersion} download + extract complete: SHA256 OK.`);

  const buildCwd = path.join(extractPath, `openssl-${openSSLVersion}`);

  if (process.platform === "darwin") {
    await buildDarwin(buildCwd, macOsDeploymentTarget);
  } else if (process.platform === "linux") {
    await buildLinux(buildCwd);
  } else if (process.platform === "win32") {
    await buildWin32(buildCwd, vsBuildArch);
  } else {
    throw new Error(`Unknown platform: ${process.platform}`);
  }

  console.log("Build finished.");
}

const downloadOpenSSLIfNecessary = async ({
  downloadBinUrl,
  maybeDownloadSha256,
  maybeDownloadSha256Url
}) => {
  if (process.platform !== "darwin" && process.platform !== "win32" && process.platform !== "linux") {
    console.log(`Skipping OpenSSL download, not required on ${process.platform}`);
    return;
  }

  if (process.platform === "linux" && process.env.NODEGIT_OPENSSL_STATIC_LINK !== "1") {
    console.log(`Skipping OpenSSL download, NODEGIT_OPENSSL_STATIC_LINK !== 1`);
    return;
  }

  try {
    await fs.stat(extractPath);
    console.log("Skipping OpenSSL download, dir exists");
    return;
  } catch {}
  
  if (maybeDownloadSha256Url) {
    maybeDownloadSha256 = (await got(maybeDownloadSha256Url)).body.trim();
  }

  const downloadStream = got.stream(downloadBinUrl);
  downloadStream.on("downloadProgress", makeOnStreamDownloadProgress());

  const pipelineSteps = [
    downloadStream,
    maybeDownloadSha256
      ? new HashVerify("sha256", makeHashVerifyOnFinal(maybeDownloadSha256))
      : null,
    zlib.createGunzip(),
    tar.extract(extractPath)
  ].filter(step => step !== null);
  await pipeline(
    ...pipelineSteps
  );

  console.log(`OpenSSL download + extract complete${maybeDownloadSha256 ? ": SHA256 OK." : "."}`);
  console.log("Download finished.");
}

const getOpenSSLPackageName = () => {
  let arch = process.arch;
  if (process.platform === "win32" && (
    process.arch === "ia32" || process.env.NODEGIT_VS_BUILD_ARCH === "x86"
  )) {
    arch = "x86";
  }

  return `openssl-${OPENSSL_VERSION}-${process.platform}-${arch}.tar.gz`;
}

const getOpenSSLPackageUrl = () => `${packageJson.binary.host}${getOpenSSLPackageName()}`;

const buildPackage = async () => {
  let resolve, reject;
  const promise = new Promise((_resolve, _reject) => {
    resolve = _resolve;
    reject = _reject;
  });
  await pipeline(
    tar.pack(extractPath, {
      entries: pathsToIncludeForPackage,
      ignore: (name) => {
        // Ignore pkgconfig files
        return path.extname(name) === ".pc"
          || path.basename(name) === "pkgconfig";
      },
      dmode: 0755,
      fmode: 0644
    }),
    zlib.createGzip(),
    new HashVerify("sha256", (digest) => {
      resolve(digest);
    }),
    fsNonPromise.createWriteStream(getOpenSSLPackageName())
  );
  const digest = await promise;
  await fs.writeFile(`${getOpenSSLPackageName()}.sha256`, digest);
};

const acquireOpenSSL = async () => {
  try {
    const downloadBinUrl = process.env.npm_config_openssl_bin_url
      || (['win32', 'darwin'].includes(process.platform) ? getOpenSSLPackageUrl() : undefined);
    if (downloadBinUrl && downloadBinUrl !== 'skip' && !process.env.NODEGIT_OPENSSL_BUILD_PACKAGE) {
      const downloadOptions = { downloadBinUrl };
      if (process.env.npm_config_openssl_bin_sha256 !== 'skip') {
        if (process.env.npm_config_openssl_bin_sha256) {
          downloadOptions.maybeDownloadSha256 = process.env.npm_config_openssl_bin_sha256;
        } else {
          downloadOptions.maybeDownloadSha256Url = `${getOpenSSLPackageUrl()}.sha256`;
        }
      }

      await downloadOpenSSLIfNecessary(downloadOptions);
      return;
    }

    let macOsDeploymentTarget;
    if (process.platform === "darwin") {
      macOsDeploymentTarget = process.argv[2];
      if (!macOsDeploymentTarget || !macOsDeploymentTarget.match(/\d+\.\d+/)) {
        throw new Error(`Invalid macOsDeploymentTarget: ${macOsDeploymentTarget}`);
      }
    }

    let vsBuildArch;
    if (process.platform === "win32") {
      vsBuildArch = process.env.NODEGIT_VS_BUILD_ARCH || (process.arch === "x64" ? "x64" : "x86");
      if (!["x64", "x86"].includes(vsBuildArch)) {
        throw new Error(`Invalid vsBuildArch: ${vsBuildArch}`);
      }
    }

    await buildOpenSSLIfNecessary({
      openSSLVersion: OPENSSL_VERSION,
      macOsDeploymentTarget,
      vsBuildArch
    });
    if (process.env.NODEGIT_OPENSSL_BUILD_PACKAGE) {
      await buildPackage();
    }
  } catch (err) {
    console.error("Acquire failed: ", err);
    process.exit(1);
  }
};

module.exports = {
  acquireOpenSSL,
  getOpenSSLPackageName,
  OPENSSL_VERSION
};

if (require.main === module) {
  acquireOpenSSL().catch((error) => {
    console.error("Acquire OpenSSL failed: ", error);
    process.exit(1);
  });
}
