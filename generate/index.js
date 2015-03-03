var generateJson = require("./scripts/generateJson");
var generateNativeCode = require("./scripts/generateNativeCode");
var generateMissingTests = require("./scripts/generateMissingTests");
var Promise = require("nodegit-promise");

module.exports = function generate() {
  return Promise.all([
    generateJson().then(function() {
      console.info('[nodegit] Generated output JSON');
    }),

    generateNativeCode().then(function() {
      console.info('[nodegit] Generated native code');
    }),

    generateMissingTests().then(function() {
      console.info('[nodegit] Generated missing tests');
    })
  ]);
}

// Invoke method if called from `node generate`.
if (require.main === module) {
  module.exports().then(function() {
    process.exit(0);
  });
}
