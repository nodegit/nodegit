var generateJson = require("./scripts/generateJson");
var generateNativeCode = require("./scripts/generateNativeCode");
var generateMissingTests = require("./scripts/generateMissingTests");
var submoduleStatus = require("../lifecycleScripts/submodules/getStatus");

module.exports = function generate() {
  return submoduleStatus()
    .then(function(statuses) {
      var dirtySubmodules = statuses
        .filter(function(status) {
          return status.onNewCommit
            || status.needsInitialization
            || status.workDirDirty;
        });

      if (dirtySubmodules.length) {
        console.log("WARNING - Some submodules are out-of-sync");
        dirtySubmodules.forEach(function(submodule) {
          console.log("\t" + submodule.name);
        });
      }
    })
    .then(function() {
      generateJson();
      generateNativeCode();
      generateMissingTests();
    })
    .catch(function(e) {
      console.log("ERROR - Could not generate native code");
      console.log(e);
    });
}

if (require.main === module) {
  module.exports();
}
