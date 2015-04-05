var NodeGit = require("../");

var Stash = NodeGit.Stash;

// Override Stash.foreach to eliminate the need to pass null payload
var foreach = Stash.foreach;
Stash.foreach = function(repo, callback) {
  return foreach(repo, callback, null);
};
