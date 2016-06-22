var path = require("path");

var buildFlags = require("../utils/buildFlags");
var exec = require("../utils/execPromise");

module.exports = function install() {
  console.log("[nodegit] Running install script");

  var nodePreGypCmd = path.join(
    __dirname,
    "..",
    "node_modules",
    ".bin",
    "node-pre-gyp"
  );

  if (process.platform === "win32") {
    nodePreGypCmd += ".cmd";
  }

  var cmd = [nodePreGypCmd, "install"];

  if (buildFlags.mustBuild) {
    console.info(
      "[nodegit] Pre-built download disabled, building from source."
    );
    cmd.push("--build-from-source");

    if (buildFlags.debugBuild) {
      console.info("[nodegit] Building debug version.");
      cmd.push("--debug");
    }
  }
  else {
    cmd.push("--fallback-to-build");
  }

  return exec(cmd.join(" "))
    .then(function() {
      console.info("[nodegit] Completed installation successfully.");
    });
};

// Called on the command line
if (require.main === module) {
  module.exports()
    .catch(function(e) {
      console.error("[nodegit] ERROR - Could not finish install");
      console.error(e);
      process.exit(1);
    });
}
