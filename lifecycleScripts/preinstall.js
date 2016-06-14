var path = require("path");
var local = path.join.bind(path, __dirname);

var configure = require(local("configureLibssh2"));
var buildFlags = require(local("../utils/buildFlags"));

module.exports = function prepareForBuild() {
  return configure()
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
  module.exports();
}
