var NodeGit = require("../");

var Stash = NodeGit.Stash;

// Override Stash.foreach to eliminate the need to pass null payload
var foreach = Stash.foreach;
Stash.foreach = function(repo, callback) {
  function wrappedCallback(index, message, oid) {
    // We need to copy the OID since libgit2 types are getting cleaned up
    // incorrectly right now in callbacks

    return callback(index, message, oid.copy());
  }

  return foreach(repo, wrappedCallback, null);
};
