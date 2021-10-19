const crypto = require("crypto");
const execPromise = require("./execPromise");
const fsNonPromise = require("fs");
const fs = fsNonPromise.promises;
const path = require("path");
const got = require("got");
const os = require("os");
const promisify = require("util").promisify;
const stream = require("stream");
const tar = require("tar-fs");
const zlib = require("zlib");

const pipeline = promisify(stream.pipeline);

const win32BatPath = path.join(__dirname, "build-openssl.bat");
const vendorPath = path.resolve(__dirname, "..", "vendor");
const distrosFilePath = path.join(vendorPath, "static_config", "openssl_distributions.json");
const extractPath = path.join(vendorPath, "openssl");

// TODO: Determine if we are GYPing in Debug
const getIsDebug = () => false;

const getOpenSSLSourceUrl = (version) => `https://www.openssl.org/source/openssl-${version}.tar.gz`;
const getOpenSSLSourceSha256Url = (version) => `${getOpenSSLSourceUrl(version)}.sha256`;

class HashVerify extends stream.Transform {
  constructor(algorithm, expected) {
    super();
    this.expected = expected;
    this.hash = crypto.createHash(algorithm);
  }
  
  _transform(chunk, encoding, callback) {
    this.hash.update(chunk, encoding);
    callback(null, chunk);
  }

  _final(callback) {
    const digest = this.hash.digest("hex");
    const digestOk = digest === this.expected;
    callback(digestOk ? null : new Error(`Digest not OK: ${digest} !== ${this.expected}`));
  }
}

const buildDarwin = async (buildCwd) => {
  await execPromise(`./Configure darwin64-x86_64-cc shared enable-ec_nistp_64_gcc_128 no-ssl2 no-ssl3 no-comp --prefix="${
    extractPath
  }" --openssldir="${extractPath}" -mmacosx-version-min=10.11`, {
    cwd: buildCwd
  }, { pipeOutput: true });

  await execPromise("make depend", {
    cwd: buildCwd
  }, { pipeOutput: true });

  await execPromise("make install", {
    cwd: buildCwd,
    maxBuffer: 10 * 1024 * 1024 // we should really just use spawn
  }, { pipeOutput: true });
};

const buildWin32 = async (buildCwd) => {
  const vcvarsPath = `${
    process.env.ProgramFiles
  }\\Microsoft Visual Studio\\2017\\BuildTools\\VC\\Auxiliary\\Build\\vcvars${
    process.arch === "x64" ? "64" : "32"
  }.bat`;
  await execPromise(`win32BatPath "${vcvarsPath}" ${process.arch === "x64" ? "VC-WIN64A" : "VC-WIN32"}`, {
    cwd: buildCwd,
    maxBuffer: 10 * 1024 * 1024 // we should really just use spawn
  }, { pipeOutput: true });
};

const buildOpenSSLIfNecessary = async (openSSLVersion) => {
  const platform = os.platform();
  if (platform !== "darwin" && platform !== "win32") {
    console.log(`Skipping OpenSSL build, not required on ${platform}`);
    return;
  }

  try {
    await fs.stat(extractPath);
    console.log("Skipping OpenSSL build, dir exists");
    return;
  } catch {}

  const openSSLUrl = getOpenSSLSourceUrl(openSSLVersion);
  const openSSLSha256Url = getOpenSSLSourceSha256Url(openSSLVersion);

  const openSSLSha256 = (await got(openSSLSha256Url)).body.trim();

  const downloadStream = got.stream(openSSLUrl);
  let lastReport = performance.now();
  downloadStream.on("downloadProgress", ({ percent, transferred, total }) => {
    const currentTime = performance.now();
    if (currentTime - lastReport > 1 * 1000) {
      lastReport = currentTime;
      console.log(`progress: ${transferred}/${total} (${(percent * 100).toFixed(2)}%)`)
    }
  });
  
  await pipeline(
    downloadStream,
    new HashVerify("sha256", openSSLSha256),
    zlib.createGunzip(),
    tar.extract(extractPath)
  );

  console.log("OpenSSL download + extract complete: SHA256 OK.");

  const buildCwd = path.join(extractPath, `openssl-${openSSLVersion}`);

  if (platform === "darwin") {
    await buildDarwin(buildCwd);
  } else if (platform === "win32") {
    await buildWin32(buildCwd);
  } else {
    throw new Error(`Unknown platform: ${platform}`);
  }

  console.log("Build finished.");
}

const acquireOpenSSL = async () => {
  try {
    await buildOpenSSLIfNecessary("1.1.1c");
  } catch (err) {
    console.error("Acquire failed: ", err);
    process.exit(1);
  }
};

acquireOpenSSL();
