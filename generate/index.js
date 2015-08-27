var Promise = require('nodegit-promise');
var generateJson = require("./scripts/generateJson");
var generateNativeCode = require("./scripts/generateNativeCode");
var generateMissingTests = require("./scripts/generateMissingTests");

module.exports = function generate() {
  return new Promise(function(resolve, reject) {
    try {
      generateJson();
      generateNativeCode();
      generateMissingTests();
      resolve();
    }
    catch(e) {
      reject(e);
    }
  });
}

if (require.main === module) {
  module.exports();
}
