var NodeGit = require("../");

var Index = NodeGit.Index;
var Status = NodeGit.Status;
var Pathspec = NodeGit.Pathspec;

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
  var repo = this.owner();
  var statusCB = function(path) {
    paths.push(path);
  };
  var idx = this;
  return Pathspec.create(pathspec || "*")
    .then(function(ps) {
      pathspec = ps;
      return Status.foreach(repo, statusCB);
    })
    .then(function() {
      return paths;
    })
    .then(function(paths) {
      paths = paths.filter(function(path) {
        return !!(pathspec.matchesPath(0, path));
      });
      return addAll.call(idx, paths, flags, matchedCallback, null);
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
