var generateJson = require("./scripts/generateJson");
var generateNativeCode = require("./scripts/generateNativeCode");
var generateMissingTests = require("./scripts/generateMissingTests");

module.exports = function generate() {
  generateJson();
  generateNativeCode();
  generateMissingTests();
}

if (require.main === module) {
  module.exports();
}
