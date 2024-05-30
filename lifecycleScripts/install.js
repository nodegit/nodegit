var buildFlags = require("../utils/buildFlags");

module.exports = async function install() {
  console.log("[nodegit] Running install script");

  var args = ["install"];

  if (buildFlags.mustBuild) {
    console.info(
      "[nodegit] Pre-built download disabled, building from source.",
    );
    args.push("--build-from-source");

    if (buildFlags.debugBuild) {
      console.info("[nodegit] Building debug version.");
      args.push("--debug");
    }
  } else {
    args.push("--fallback-to-build");
  }

  return;
};

// Called on the command line
if (require.main === module) {
  module.exports().catch(function (e) {
    console.error("[nodegit] ERROR - Could not finish install");
    console.error("[nodegit] ERROR - finished with error code: " + e);
    process.exit(e);
  });
}
