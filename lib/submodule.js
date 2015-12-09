var NodeGit = require("../");

var Submodule = NodeGit.Submodule;

// Override Submodule.foreach to eliminate the need to pass null payload
var foreach = Submodule.foreach;
Submodule.foreach = function(repo, callback) {
  return foreach(repo, callback, null);
};
