var NodeGit = require("../");

var Submodule = NodeGit.Submodule;

var _foreach = Submodule.foreach;

// Override Submodule.foreach to eliminate the need to pass null payload
Submodule.foreach = function(repo, callback) {
  return _foreach(repo, callback, null);
};
