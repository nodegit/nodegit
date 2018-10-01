var path = require("path");
var rootDir = path.join(__dirname, "../..");

var gitExecutableLocation = require(
  path.join(rootDir, "./utils/gitExecutableLocation")
);
var submoduleStatus = require("./getStatus");

var exec = require(path.join(rootDir, "./utils/execPromise"));

module.exports = function submodules() {
  return gitExecutableLocation()
    .catch(function() {
      console.error("[nodegit] ERROR - Compilation of NodeGit requires git " +
        "CLI to be installed and on the path");

      throw new Error("git CLI is not installed or not on the path");
    })
    .then(function() {
      console.log("[nodegit] Checking submodule status");
      return submoduleStatus();
    })
    .then(function(statuses) {
      function printSubmodule(submoduleName) {
        console.log("\t" + submoduleName);
      }

      var dirtySubmodules = statuses
      .filter(function(status) {
        return status.workDirDirty && !status.needsInitialization;
      })
      .map(function(dirtySubmodule) {
        return dirtySubmodule.name;
      });

      if (dirtySubmodules.length) {
        console.error(
          "[nodegit] ERROR - Some submodules have uncommited changes:"
        );
        dirtySubmodules.forEach(printSubmodule);
        console.error(
          "\nThey must either be committed or discarded before we build"
        );

        throw new Error("Dirty Submodules: " + dirtySubmodules.join(" "));
      }

      var outOfSyncSubmodules = statuses
        .filter(function(status) {
          return status.onNewCommit && !status.needsInitialization;
        })
        .map(function(outOfSyncSubmodule) {
          return outOfSyncSubmodule.name;
        });

      if (outOfSyncSubmodules.length) {
        console.warn(
          "[nodegit] WARNING - Some submodules are pointing to an new commit:"
        );
        outOfSyncSubmodules.forEach(printSubmodule);
        console.warn("\nThey will not be updated.");
      }

      return statuses
        .filter(function(status) {
          return !status.onNewCommit;
        })
        .reduce(function(chainPromise, submoduleToUpdate) {
          return chainPromise
            .then(function() {
              console.log(
                "[nodegit] Initializing submodule",
                submoduleToUpdate.name
              );
              return exec(
                "git submodule update --init --recursive " +
                submoduleToUpdate.name
              );
            });
        }, Promise.resolve());
    });
};
