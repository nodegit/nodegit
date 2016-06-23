var buildFlags = require("../utils/buildFlags");
var spawn = require("child_process").spawn;

module.exports = function install() {
  console.log("[nodegit] Running install script");

  var nodePreGyp = "node-pre-gyp";

  if (process.platform === "win32") {
    nodePreGyp += ".cmd";
  }

  var args = ["install"];

  if (buildFlags.mustBuild) {
    console.info(
      "[nodegit] Pre-built download disabled, building from source."
    );
    args.push("--build-from-source");

    if (buildFlags.debugBuild) {
      console.info("[nodegit] Building debug version.");
      args.push("--debug");
    }
  }
  else {
    args.push("--fallback-to-build");
  }

  return new Promise(function(resolve, reject) {
    var spawnedNodePreGyp = spawn(nodePreGyp, args);

    spawnedNodePreGyp.stdout.on("data", function(data) {
      console.info(data.toString().trim());
    });

    spawnedNodePreGyp.stderr.on("data", function(data) {
      console.error(data.toString().trim());
    });

    spawnedNodePreGyp.on("close", function(code) {
      if (!code) {
        resolve();
      } else {
        reject(code);
      }
    });
  })
    .then(function() {
      console.info("[nodegit] Completed installation successfully.");
    });
};

// Called on the command line
if (require.main === module) {
  module.exports()
    .catch(function(e) {
      console.error("[nodegit] ERROR - Could not finish install");
      console.error("[nodegit] ERROR - finished with error code: " + e);
      process.exit(e);
    });
}
