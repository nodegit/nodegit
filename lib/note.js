var NodeGit = require("../");

var Note = NodeGit.Note;

var _foreach = Note.foreach;

// Override Note.foreach to eliminate the need to pass null payload
Note.foreach = function(repo, notesRef, callback) {
  function wrapperCallback(blobId, objectId) {
    // We need to copy the OID since libgit2 types are getting cleaned up
    // incorrectly right now in callbacks

    return callback(blobId.copy(), objectId.copy());
  }

  return _foreach(repo, notesRef, wrapperCallback, null);
};
