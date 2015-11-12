var path = require("path");

var exec = require("./exec");

module.exports = function build(action, targetInfo) {
  var opts = {
    cwd: ".",
    maxBuffer: Number.MAX_VALUE,
    env: process.env
  };

  var debug = (process.env.BUILD_DEBUG ? " --debug" : "");
  var distUrl = "";

  if (targetInfo.target === "electron") {
    var home = process.platform == "win32" ?
            process.env.USERPROFILE : process.env.HOME;
    opts.env.HOME = path.join(home, ".electron-gyp");

    distUrl = "--dist-url=https://atom.io/download/atom-shell";
  }

  var builder = path.resolve(".", "node_modules", ".bin", "node-pre-gyp");
  builder = builder.replace(/\s/g, "\\$&");
  var cmd = [builder, action, debug, distUrl, "--build-from-source"]
    .concat(targetInfo.args)
    .join(" ")
    .trim();

  return exec(cmd, opts);
};
