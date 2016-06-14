var path = require("path");
var local = path.join.bind(path, __dirname);

var exec = require(local("../utils/execPromise"));
var configure = require(local("configureLibssh2"));
var buildFlags = require(local("../utils/buildFlags"));

module.exports = function prepareForBuild() {
  console.log("[nodegit] Running pre-install script");

  return exec("npm -v")
    .then(function(npmVersion) {
      if (npmVersion.split(".")[0] < 3) {
        console.log("[nodegit] npm@2 installed, pre-loading required packages");
        return exec("npm install --ignore-scripts");
      }

      return Promise.resolve();
    })
    .then(function() {
      return configure();
    })
    .then(function() {
      if (buildFlags.isGitRepo) {
        var submodules = require(local("submodules"));
        var generate = require(local("../generate"));
        return submodules()
          .then(function() {
            return generate();
          });
      }
    });
};

// Called on the command line
if (require.main === module) {
  module.exports()
    .catch(function(e) {
      console.error("[nodegit] ERROR - Could not finish preinstall");
      console.error(e);
      process.exit(1);
    });
}
