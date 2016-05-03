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
      console.log("ERROR - Compilation of NodeGit requires git CLI to be " +
        "installed and on the path");

      throw new Error("git CLI is not installed or not on the path");
    })
    .then(function() {
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
        console.log(
          "ERROR - The following submodules have uncommited changes:"
        );
        dirtySubmodules.forEach(printSubmodule);
        console.log(
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
        console.log(
          "WARNING - The following submodules are pointing to an new commit:"
        );
        outOfSyncSubmodules.forEach(printSubmodule);
        console.log("\nThey will not be updated.");
      }

      return Promise.all(statuses
        .filter(function(status) {
          return !status.onNewCommit;
        })
        .map(function(submoduleToUpdate) {
          return exec(
            "git submodule update --init --recursive " + submoduleToUpdate.name
          );
        })
      );
    });
};
