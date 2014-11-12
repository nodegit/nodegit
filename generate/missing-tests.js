const path = require("path");
const idefs = require("./idefs");
const Promise = require("nodegit-promise");
const promisify = require("promisify-node");
const fse = promisify(require("fs-extra"));
const testFilesPath = path.resolve(__dirname, "../test/tests");

var output = {};

function findMissingTest(idef) {
  var testFilePath = path.join(testFilesPath, idef.filename + ".js");
  var result = {};

  return fse.readFile(testFilePath, "utf8")
  .then(function(file) {
    var fieldsResult = [];
    var functionsResult = [];

    idef.fields.forEach(function(field) {
      if (file.indexOf(field.jsFunctionName) < 0) {
        fieldsResult.push(field.jsFunctionName);
      }
    });

    result.fields = fieldsResult;

    idef.functions.forEach(function(fn) {
      if (file.indexOf(fn.jsFunctionName) < 0) {
        functionsResult.push(fn.jsFunctionName);
      }
    });

    result.functions = functionsResult;
  },
  function() {
    result.testFileMissing = false;
    result.testFilePath = testFilePath;
  }).then(function() {
    output[idef.filename] = result;
  });
};

var promises = [];

idefs.forEach(function(idef) {
  promises.push(findMissingTest(idef));
});

Promise.all(promises)
.then(function() {
  fse.writeFileSync(path.join(__dirname, "missing-tests.json"),
    JSON.stringify(output, null, 2));
});
