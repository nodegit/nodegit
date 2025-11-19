var fs = require("fs");
var path = require("path");

var isGitRepo;

try {
  fs.statSync(path.join(__dirname, "..", ".git"));
  isGitRepo = true;
} catch (e) {
  isGitRepo = false;
}

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

module.exports = {
  hostArch,
  targetArch,
  debugBuild: !!process.env.BUILD_DEBUG,
  isElectron: process.env.npm_config_runtime === "electron",
  isGitRepo: isGitRepo,
  isNwjs: process.env.npm_config_runtime === "node-webkit",
  mustBuild: !!(isGitRepo || process.env.BUILD_DEBUG || process.env.BUILD_ONLY)
};
