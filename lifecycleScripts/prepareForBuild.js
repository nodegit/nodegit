var Promise = require("nodegit-promise");
var cp = require("child_process");
var path = require("path");

var local = path.join.bind(path, __dirname);

var configure = require(local("configureLibssh2"));
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
    return new Promise(function(resolve, reject) {

      configure().catch(function(e) {
         console.info("[nodegit] Configuration malfunctioned:");
         console.log(e || "[nodegit] No Configuration exception.");
         reject();
      }).then(function() {
         console.info("[nodegit] Configuration complete.");
         return generate();
      }, function(e) {
         console.info("[nodegit] Configuration failed:");
         console.log(e || "[nodegit] No Configuration failure exception.");
         reject();
      }).catch(function(e) {
         console.info("[nodegit] Generation malfunctioned:");
         console.log(e || "[nodegit] No Generation exception.");
         reject();
      }).then(function() {
         console.info("[nodegit] Generation complete.");
         resolve();
      }, function(e) {
         console.info("[nodegit] Generation failed:");
         console.log(e || "[nodegit] No Generation failure exception.");
         reject();
      });

    });

  });
};

// Called on the command line
if (require.main === module) {
  module.exports();
}
