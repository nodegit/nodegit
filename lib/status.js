var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Status = NodeGit.Status;

var _foreach = Status.foreach;
var _foreachExt = Status.foreachExt;

// Override Status.foreach to eliminate the need to pass null payload
Status.foreach = function(repo, callback) {
  return _foreach(repo, callback, null);
};

// Override Status.foreachExt to normalize opts
Status.foreachExt = function(repo, opts, callback) {
  opts = normalizeOptions(opts, NodeGit.StatusOptions);
  return _foreachExt(repo, opts, callback, null);
};
