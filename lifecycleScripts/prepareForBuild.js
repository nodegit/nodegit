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
    return Promise.all([
      configure(),
      generate()
    ]);
  });
};

// Called on the command line
if (require.main === module) {
  module.exports();
}
