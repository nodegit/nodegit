var fse = require("fs-extra");
var path = require("path");

var exec = require("../utils/execPromise");
var buildFlags = require("../utils/buildFlags");

var rootPath = path.join(__dirname, "..");

function printStandardLibError() {
  console.log(
    "[ERROR] Seems like the latest libstdc++ is missing on your system!"
  );
  console.log("");
  console.log("On Ubuntu you can install it using:");
  console.log("");
  console.log("$ sudo add-apt-repository ppa:ubuntu-toolchain-r/test");
  console.log("$ sudo apt-get update");
  console.log("$ sudo apt-get install libstdc++-4.9-dev");
}

module.exports = function install() {
  return exec("node dist/nodegit.js")
    .then(function() {
      // Is we're using NodeGit from a package manager then let's clean up after
      // ourselves when we install successfully.
      if (!buildFlags.mustBuild) {
        fse.removeSync(path.join(rootPath, "vendor"));
        fse.removeSync(path.join(rootPath, "src"));
        fse.removeSync(path.join(rootPath, "include"));
        fse.removeSync(path.join(rootPath, "build/Release/*.a"));
        fse.removeSync(path.join(rootPath, "build/Release/obj.target"));
      }
    });
};

// Called on the command line
if (require.main === module) {
  module.exports()
    .catch(function(e) {
      console.error("[nodegit] ERROR - Could not finish postinstall");

      if (process.pladtform !== "linux" && ~e.indexOf("libstdc++")) {
        printStandardLibError();
      }
      else {
        console.error(e);
      }

      process.exit(1);
    });
}
