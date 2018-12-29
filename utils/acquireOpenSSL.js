const fse = require("fs-extra");
const path = require("path");
const R = require("ramda");
const request = require("request-promise-native");
const stream = require("stream");
const tar = require("tar-fs");
const zlib = require("zlib");

const vendorPath = path.resolve(__dirname, "..", "vendor");
const distrosFilePath = path.join(vendorPath, "static_config", "openssl_distributions.json");
const extractPath = path.join(vendorPath, "openssl");

const getOSName = () => {
  if (process.platform === "win32") {
    if (process.arch === "x64") {
      return "win64";
    } else {
      return "win32";
    }
  } else if (process.platform === "darwin") {
    return "macOS";
  } else {
    // We only discover distros for Mac and Windows. We don't care about any other OS.
    return "unknown";
  }
};

const getCompilerVersion = () => {
  // TODO: Get actual compiler version. For now, just assume latest compiler for distros in openssl_distributions.js
  const osName = getOSName();
  if (osName === "win32" || osName === "win64") {
    return "vs14";
  } else if (osName === "macOS") {
    return "clang-9";
  } else {
    // We only discover distros for Mac and Windows. We don't care about any other OS.
    return "unknown";
  }
};

// TODO: Determine if we are GYPing in Debug
const getIsDebug = () => false;

const getMatchingDistributionName = () =>
  `${getOSName()}-${getCompilerVersion()}-static${getIsDebug() ? "-debug" : "-release"}`;

const getDistributionsConfig = () =>
  fse.readFile(distrosFilePath, "utf8")
    .then(JSON.parse);

const getDistrbutionURLFromConfig = (config) => {
  const distName = getMatchingDistributionName();
  const distURL = R.propOr(null, distName, config);

  if (!distURL) {
    return Promise.reject(new Error("No matching distribution for this operating system"));
  }
  return Promise.resolve(distURL);
};

const fetchFileFromURL = (distUrl) => request({
  method: "GET",
  uri: distUrl,
  encoding: null,
  gzip: true
});

const extractFile = (body) => new Promise((resolve, reject) => {
  const streamableBody = new stream.Readable();
  streamableBody.push(body);
  streamableBody.push(null);
  streamableBody
    .pipe(zlib.createGunzip())
    .on("error", reject)
    .pipe(tar.extract(extractPath))
    .on("error", reject)
    .on("close", resolve);
});

const acquireOpenSSL = () =>
  getDistributionsConfig()
    .then(getDistrbutionURLFromConfig)
    .then(fetchFileFromURL)
    .then(extractFile)
    .catch((e) => {
      console.error(e);
      process.exit(1);
    });

acquireOpenSSL();
