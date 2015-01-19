var Promise = require("nodegit-promise");
var cp = require("child_process");
var path = require("path");

var local = path.join.bind(path, __dirname);

var check = require(local("checkPrepared")).checkGenerated;
var retrieve = require(local("retrieveExternalDependencies"));
var generate = require(local("../generate"));

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
    if (allThere) {
      console.info("[nodegit] Generated code is intact.");
      return Promise.resolve();
    }
    else {
      console.info("[nodegit] Generated code is missing or incomplete, " +
        "regenerating now.");

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
    }
  });
}
// Called on the command line
if (require.main === module) {
  module.exports();
}
