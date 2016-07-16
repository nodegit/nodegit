var NodeGit = require("../");

var Index = NodeGit.Index;

var addAll = Index.prototype.addAll;
Index.prototype.addAll = function(pathspec, flags, matchedCallback) {
  return addAll.call(this, pathspec || "*", flags, matchedCallback, null);
};

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

var removeAll = Index.prototype.removeAll;
Index.prototype.removeAll = function(pathspec, matchedCallback) {
  return removeAll.call(this, pathspec || "*", matchedCallback, null);
};

var updateAll = Index.prototype.updateAll;
Index.prototype.updateAll = function(pathspec, matchedCallback) {
  return updateAll.call(this, pathspec || "*", matchedCallback, null);
};
