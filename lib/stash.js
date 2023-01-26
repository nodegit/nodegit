var NodeGit = require("../");
var Stash = NodeGit.Stash;

var _foreach = Stash.foreach;

// Override Stash.foreach to eliminate the need to pass null payload
Stash.foreach = function (repo, callback) {
  function wrappedCallback(index, message, oid) {
    // We need to copy the OID since libgit2 types are getting cleaned up
    // incorrectly right now in callbacks

    return callback(index, message, oid.copy());
  }

  return _foreach(repo, wrappedCallback, null);
};
