var NodeGit = require("../");

var Index = NodeGit.Index;
var Status = NodeGit.Status;
var Promise = require("nodegit-promise");
/**
 * Return an array of the entries in this index.
 * @return {Array<IndexEntry>} an array of IndexEntrys
 */
Index.prototype.entries = function() {
  var size = this.entryCount();
  var result = [];

  for (var i = 0; i < size; i++) {
    result.push(this.getByIndex(i));
  }

  return result;
};

var addAll = Index.prototype.addAll;
Index.prototype.addAll = function(pathspec, flags, matchedCallback) {
  // This status path code is here to speedup addall, which currently is
  // excessively slow due to adding every single unignored file to the index
  // even if it has no changes. Remove this when it's fixed in libgit2
  // https://github.com/libgit2/libgit2/issues/2687
  var paths = [];
  var statusCB = function(path) {
    paths.push(path);
  };
  return Status.foreach(this.owner(), statusCB)
    .then(function() {
      return Promise.resolve(paths);
    })
    .then(function(paths) {
      return addAll.call(this, pathspec || "*", flags, matchedCallback, null);
    });
};

var removeAll = Index.prototype.removeAll;
Index.prototype.removeAll = function(pathspec, matchedCallback) {
  return removeAll.call(this, pathspec || "*", matchedCallback, null);
};

var updateAll = Index.prototype.updateAll;
Index.prototype.updateAll = function(pathspec, matchedCallback) {
  return updateAll.call(this, pathspec || "*", matchedCallback, null);
};
