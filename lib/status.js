var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Status = NodeGit.Status;

// Override Status.foreach to eliminate the need to pass null payload
var foreach = Status.foreach;
Status.foreach = function(repo, callback) {
  return foreach(repo, callback, null);
};

// Override Status.foreachExt to normalize opts
var foreachExt = Status.foreachExt;
Status.foreachExt = function(repo, opts, callback) {
  opts = normalizeOptions(opts, NodeGit.StatusOptions);
  return foreachExt(repo, opts, callback, null);
};
