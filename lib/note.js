var NodeGit = require("../");

var Note = NodeGit.Note;

// Override Note.foreach to eliminate the need to pass null payload
var foreach = Note.foreach;
Note.foreach = function(repo, notesRef, callback) {
  return foreach(repo, notesRef, callback, null);
};
