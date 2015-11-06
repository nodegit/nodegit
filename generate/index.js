var Promise = require('nodegit-promise');
var generateJson = require("./scripts/generateJson");
var generateNativeCode = require("./scripts/generateNativeCode");
var generateMissingTests = require("./scripts/generateMissingTests");

module.exports = function generate() {
  return new Promise(function(resolve, reject) {
    console.info("[nodegit] Generating started.");
    try {
      generateJson();
      generateNativeCode();
      generateMissingTests();
      console.info("[nodegit] Generating completed.");
      resolve();
    }
    catch(e) {
      console.info("[nodegit] Generating failed.");
      reject(e);
    }
  });
}

if (require.main === module) {
  module.exports();
}
