var fse = require("fs-extra");
var path = require("path");

var exec = require("../utils/execPromise");
var buildFlags = require("../utils/buildFlags");

var rootPath = path.join(__dirname, "..");

function printStandardLibError() {
  console.log(
    "[nodegit] ERROR - the latest libstdc++ is missing on your system!"
  );
  console.log("");
  console.log("On Ubuntu you can install it using:");
  console.log("");
  console.log("$ sudo add-apt-repository ppa:ubuntu-toolchain-r/test");
  console.log("$ sudo apt-get update");
  console.log("$ sudo apt-get install libstdc++-4.9-dev");
}

module.exports = function install() {
  if (buildFlags.isGitRepo) {
    // If we're building NodeGit from a git repo we aren't going to do any
    // cleaning up
    return Promise.resolve();
  }
  if (buildFlags.isElectron || buildFlags.isNWjs) {
    // If we're building for electron or NWjs, we're unable to require the
    // built library so we have to just assume success, unfortunately.
    return Promise.resolve();
  }

  return exec("node \"" + path.join(rootPath, "dist/nodegit.js\""))
    .catch(function(e) {
      if (~e.toString().indexOf("Module version mismatch")) {
        console.warn(
          "[nodegit] WARN - NodeGit was built for a different version of node."
        );
        console.warn(
          "If you are building NodeGit for electron/nwjs you can " +
          "ignore this warning."
        );
      }
      else {
        throw e;
      }
    })
    .then(function() {
      // Is we're using NodeGit from a package manager then let's clean up after
      // ourselves when we install successfully.
      if (!buildFlags.mustBuild) {
        // We can't remove the source files yet because apparently the
        // "standard workflow" for native node moduels in Electron/nwjs is to
        // build them for node and then nah eff that noise let's rebuild them
        // again for the actual platform! Hurray!!! When that madness is dead
        // we can clean up the source which is a serious amount of data.
        // fse.removeSync(path.join(rootPath, "vendor"));
        // fse.removeSync(path.join(rootPath, "src"));
        // fse.removeSync(path.join(rootPath, "include"));

        fse.removeSync(path.join(rootPath, "build/Release/*.a"));
        fse.removeSync(path.join(rootPath, "build/Release/obj.target"));
      }
    });
};

// Called on the command line
if (require.main === module) {
  module.exports()
    .catch(function(e) {
      console.warn("[nodegit] WARN - Could not finish postinstall");

      if (
        process.platform === "linux" &&
        ~e.toString().indexOf("libstdc++")
      ) {
        printStandardLibError();
      }
      else {
        console.log(e);
      }
    });
}
