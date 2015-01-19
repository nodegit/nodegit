const path = require("path");
const Promise = require("nodegit-promise");
const promisify = require("promisify-node");
const fse = promisify(require("fs-extra"));
const utils = require("./utils");

const testFilesPath = "../test/tests";
const missingFileIgnores = require("../input/ignored-missing-tests");

module.exports = function generateMissingTests() {
  var output = {};

  function findMissingTest(idef) {
    return new Promise(function(resolve, reject) {
      var testFilePath = path.join(testFilesPath, idef.filename + ".js");
      var result = {};

      var file = utils.readFile(testFilePath);
      if (file) {
        var fieldsResult = [];
        var functionsResult = [];
        var fieldIgnores = (missingFileIgnores[idef.filename] || {}).fields;
        var functionIgnores = (missingFileIgnores[idef.filename] || {}).functions;

        fieldIgnores = fieldIgnores || [];
        functionIgnores = functionIgnores || [];
        file = file || "";

        idef.fields.forEach(function(field) {
          if (file.indexOf(field.jsFunctionName) < 0
            && fieldIgnores.indexOf(field.jsFunctionName < 0)) {
                fieldsResult.push(field.jsFunctionName);
              }
        });

        result.fields = fieldsResult;

        idef.functions.forEach(function(fn) {
          if (file.indexOf(fn.jsFunctionName) < 0
            && functionIgnores.indexOf(fn.jsFunctionName) < 0) {
                functionsResult.push(fn.jsFunctionName);
              }
        });

        result.functions = functionsResult;
      }
      else {
        result.testFileMissing = false;
        result.testFilePath = testFilePath;
      }

      output[idef.filename] = result;
      resolve();
    });
  };

  const idefs = require("../output/idefs");
  var promises = idefs.map(function(idef) {
    return findMissingTest(idef);
  });

  Promise.all(promises).then(
    function() {
      utils.writeFile("output/missing-tests.json", output);
    },
    function(fail) {
      console.error(fail);
    }
  );

};

if (require.main === module) {
  module.exports();
}
