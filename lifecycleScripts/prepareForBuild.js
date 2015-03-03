var Promise = require("nodegit-promise");
var cp = require("child_process");

var retrieve = require("./retrieveExternalDependencies");
var check = require("./checkPrepared").checkGenerated;
var generate = require("../generate");

module.exports = function prepareForBuild() {

  return new Promise(function(resolve, reject) {
    cp.exec("npm install --ignore-scripts", function(err, stdout, stderr) {
      if (err) {
        console.error(stderr);
        reject(err, stderr);
      }
      else {
        resolve();
        console.info(stdout);
      }
    });
  }).then(function() {
    return Promise.all([
      retrieve(),
      doGenerate()
    ]);
  });
};

function doGenerate() {
  console.info("[nodegit] Detecting generated code.");
  return check().then(function(allThere) {
    return new Promise(function(resolve, reject) {
      try {
        generate();
        console.info("[nodegit] Code regenerated.");
        resolve();
      }
      catch (e) {
        console.info("[nodegit] Error generating code.");
        console.info(e);
        //console.info(stderr);
        reject(e);
      }
    });
  });
}
// Called on the command line
if (require.main === module) {
  module.exports();
}
