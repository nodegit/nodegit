import crypto from "crypto";
import { spawn } from "child_process";
import execPromise from "./execPromise.js";
import got from "got";
import path from "path";
import stream from "stream";
import tar from "tar-fs";
import zlib from "zlib";
import { createWriteStream, promises as fs } from "fs";
import { performance } from "perf_hooks";
import { promisify } from "util";

const pipeline = promisify(stream.pipeline);

import packageJson from '../package.json' with { type: "json" };

const OPENSSL_VERSION = "3.0.18";
const win32BatPath = path.join(import.meta.dirname, "build-openssl.bat");
const vendorPath = path.resolve(import.meta.dirname, "..", "vendor");
const opensslPatchPath = path.join(vendorPath, "patches", "openssl");
const extractPath = path.join(vendorPath, "openssl");

const exists = (filePath) => fs.stat(filePath).then(() => true).catch(() => false);

const convertArch = (archStr) => {
  const convertedArch = {
    'ia32': 'x86',
    'x86': 'x86',
    'x64': 'x64',
    'arm64': 'arm64'
  }[archStr];

  if (!convertedArch) {
    throw new Error('unsupported architecture');
  }

  return convertedArch;
}

const hostArch = convertArch(process.arch);
const targetArch = process.env.npm_config_arch
  ? convertArch(process.env.npm_config_arch)
  : hostArch;

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
    await fs.access(opensslPatchPath);

    for (const patchFilename of await fs.readdir(opensslPatchPath)) {
      const patchTarget = patchFilename.split("-")[1];
      if (patchFilename.split(".").pop() === "patch" && (patchTarget === operatingSystem || patchTarget === "all")) {
        console.log(`applying ${patchFilename}`);
        await execPromise(`patch -up0 -i ${path.join(opensslPatchPath, patchFilename)}`, {
          cwd: buildCwd
        }, { pipeOutput: true });
      }
    }
  } catch(e) {
    if (e.code === "ENOENT") {
      // no patches to apply
      return;
    }

    console.log("Patch application failed: ", e);
    throw e;
  }
}

const buildDarwin = async (buildCwd, macOsDeploymentTarget) => {
  if (!macOsDeploymentTarget) {
    throw new Error("Expected macOsDeploymentTarget to be specified");
  }

  const buildConfig = targetArch === "x64" ? "darwin64-x86_64-cc" : "darwin64-arm64-cc";

  const configureArgs = [
    buildConfig,
    // speed up ecdh on little-endian platforms with 128bit int support
    "enable-ec_nistp_64_gcc_128",
    // compile static libraries
    "no-shared",
    // disable ssl2, ssl3, and compression
    "no-ssl2",
    "no-ssl3",
    "no-comp",
    // disable tty ui since it fails a bunch of tests on GHA runners and we're just gonna link anyways
    "no-ui-console",
    // set install directory
    `--prefix="${extractPath}"`,
    `--openssldir="${extractPath}"`,
    // set macos version requirement
    `-mmacosx-version-min=${macOsDeploymentTarget}`
  ];

  await execPromise(`./Configure ${configureArgs.join(" ")}`, {
    cwd: buildCwd
  }, { pipeOutput: true });

  await applyOpenSSLPatches(buildCwd, "darwin");

  // only build the libraries, not the fuzzer or apps
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
  const buildConfig = targetArch === "x64" ? "linux-x86_64" : "linux-aarch64";

  const configureArgs = [
    buildConfig,
    // Electron(at least on centos7) imports the libcups library at runtime, which has a
    // dependency on the system libssl/libcrypto which causes symbol conflicts and segfaults.
    // To fix this we need to hide all the openssl symbols to prevent them from being overridden
    // by the runtime linker.
    // "-fvisibility=hidden",
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
  await execPromise(`./Configure ${configureArgs.join(" ")}`, {
    cwd: buildCwd
  }, { pipeOutput: true });

  await applyOpenSSLPatches(buildCwd, "linux");

  // only build the libraries, not the fuzzer or apps
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

const buildWin32 = async (buildCwd) => {
  let vcvarsallPath = undefined;

  if (process.env.npm_config_vcvarsall_path && await exists(process.env.npm_config_vcvarsall_path)) {
    vcvarsallPath = process.env.npm_config_vcvarsall_path;
  } else {
    const potentialMsvsPaths = [];

    // GYP_MSVS_OVERRIDE_PATH is set by node-gyp so this should cover most cases
    if (process.env.GYP_MSVS_OVERRIDE_PATH) {
      potentialMsvsPaths.push(process.env.GYP_MSVS_OVERRIDE_PATH);
    }

    const packageTypes = ["BuildTools", "Community", "Professional", "Enterprise"];
    const versions = ["2022", "2019"]

    const computePossiblePaths = (parentPath) => {
      let possiblePaths = []
      for (const packageType of packageTypes) {
        for (const version of versions) {
          possiblePaths.push(path.join(parentPath, version, packageType));
        }
      }

      return possiblePaths;
    }
    
    if (process.env["ProgramFiles(x86)"]) {
      const parentPath  = path.join(process.env["ProgramFiles(x86)"], 'Microsoft Visual Studio');
      potentialMsvsPaths.push(...computePossiblePaths(parentPath));
    }

    if (process.env.ProgramFiles) {
      const parentPath  = path.join(process.env.ProgramFiles, 'Microsoft Visual Studio');
      potentialMsvsPaths.push(...computePossiblePaths(parentPath));
    }

    for (const potentialPath of potentialMsvsPaths) {
      const wholePath = path.join(potentialPath, 'VC', 'Auxiliary', 'Build', 'vcvarsall.bat');
      console.log("checking", wholePath);
      if (await exists(wholePath)) {
        vcvarsallPath = wholePath;
        break;
      }
    }

    if (!vcvarsallPath) {
      throw new Error(`vcvarsall.bat not found`);
    }
  }

  let vcTarget;
  switch (targetArch) {
    case "x64":
      vcTarget = "VC-WIN64A";
      break;

    case "x86":
      vcTarget = "VC-WIN32";
      break;

    case "arm64":
      vcTarget = "VC-WIN64-ARM";
      break;
  }

  let vsBuildArch = hostArch === targetArch
    ? hostArch
    : `${hostArch}_${targetArch}`;

  console.log("Using vcvarsall.bat at: ", vcvarsallPath);
  console.log("Using vsBuildArch: ", vsBuildArch);
  console.log("Using vcTarget: ", vcTarget);

  await new Promise((resolve, reject) => {
    const buildProcess = spawn(`"${win32BatPath}" "${vcvarsallPath}" ${vsBuildArch} ${vcTarget}`, {
      cwd: buildCwd,
      shell: process.platform === "win32",
      env: {
        ...process.env,
        NODEGIT_SKIP_TESTS: targetArch !== hostArch ? "1" : undefined
      }
    });

    buildProcess.stdout.on("data", function(data) {
      console.info(data.toString().trim());
    });

    buildProcess.stderr.on("data", function(data) {
      console.error(data.toString().trim());
    });

    buildProcess.on("close", function(code) {
      if (!code) {
        resolve();
      } else {
        reject(code);
      }
    });
  });

  
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
    await fs.rm(extractPath, { recursive: true, force: true });
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
  openSSLVersion
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

  const openSSLSha256 = (await got(openSSLSha256Url)).body.trim().split(' ')[0];

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
    await buildWin32(buildCwd);
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

export const getOpenSSLPackageName = () => {
  return `openssl-${OPENSSL_VERSION}-${process.platform}-${targetArch}.tar.gz`;
}

export const getOpenSSLPackagePath = () => path.join(import.meta.dirname, getOpenSSLPackageName());

const getOpenSSLPackageUrl = () => {
  const hostUrl = new URL(packageJson.binary.host);
  hostUrl.pathname = getOpenSSLPackageName();
  return hostUrl.toString();
};

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
      dmode: 0o0755,
      fmode: 0o0644
    }),
    zlib.createGzip(),
    new HashVerify("sha256", (digest) => {
      resolve(digest);
    }),
    createWriteStream(getOpenSSLPackagePath())
  );
  const digest = await promise;
  await fs.writeFile(`${getOpenSSLPackagePath()}.sha256`, digest);
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
      macOsDeploymentTarget = process.argv[2] ?? process.env.OPENSSL_MACOS_DEPLOYMENT_TARGET
      if (!macOsDeploymentTarget || !macOsDeploymentTarget.match(/\d+\.\d+/)) {
        throw new Error(`Invalid macOsDeploymentTarget: ${macOsDeploymentTarget}`);
      }
    }

    await buildOpenSSLIfNecessary({
      openSSLVersion: OPENSSL_VERSION,
      macOsDeploymentTarget
    });
    if (process.env.NODEGIT_OPENSSL_BUILD_PACKAGE) {
      await buildPackage();
    }
  } catch (err) {
    console.error("Acquire failed: ", err);
    process.exit(1);
  }
};

if (process.argv[1] === import.meta.filename) {
  try {
    await acquireOpenSSL();
  }
  catch(error) {
    console.error("Acquire OpenSSL failed: ", error);
    process.exit(1);
  }
}
