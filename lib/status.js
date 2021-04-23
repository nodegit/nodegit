var NodeGit = require("../");

var Status = NodeGit.Status;

var _foreach = Status.foreach;
var _foreachExt = Status.foreachExt;

// Override Status.foreach to eliminate the need to pass null payload
Status.foreach = function(repo, callback) {
  return _foreach(repo, callback, null);
};

// Override Status.foreachExt to eliminate the need to pass null payload
Status.foreachExt = function(repo, opts, callback) {
  return _foreachExt(repo, opts, callback, null);
};
