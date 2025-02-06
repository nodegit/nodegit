const generateJson = require("./scripts/generateJson");
const generateNativeCode = require("./scripts/generateNativeCode");
const generateMissingTests = require("./scripts/generateMissingTests");
const submoduleStatus = require("../lifecycleScripts/submodules/getStatus");

module.exports = async function generate() {
  console.log("[nodegit] Generating native code");

  function tryGenerate(numRetries = 3) {
    // There appears to be a race condition in the generate code somewhere
    // Until we fix that, we should try to generate a few times before
    try {
      generateJson();
      generateNativeCode();
      generateMissingTests();
    } catch (error) {
      if (numRetries > 0) {
        console.log(
          "[nodegit] WARNING - Failed to generate native code, trying again"
        );
        tryGenerate(numRetries - 1);
      } else {
        throw error;
      }
    }
  }

  try {
    const statuses = await submoduleStatus();
    const dirtySubmodules = statuses.filter(
      (status) =>
        status.onNewCommit || status.needsInitialization || status.workDirDirty
    );

    if (dirtySubmodules.length) {
      console.warn("[nodegit] WARNING - Some submodules are out-of-sync");
      dirtySubmodules.forEach(function (submodule) {
        console.warn("[nodegit]\t" + submodule.name);
      });
    }

    await tryGenerate();
  } catch (e) {
    console.error("[nodegit] ERROR - Could not generate native code");
    console.error(e);
    throw e;
  }
};

if (require.main === module) {
  module.exports();
}
