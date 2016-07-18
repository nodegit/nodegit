var NodeGit = require("../");

var Index = NodeGit.Index;

var _addAll = Index.prototype.addAll;
var _removeAll = Index.prototype.removeAll;
var _updateAll = Index.prototype.updateAll;

Index.prototype.addAll = function(pathspec, flags, matchedCallback) {
  return _addAll.call(this, pathspec || "*", flags, matchedCallback, null);
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

Index.prototype.removeAll = function(pathspec, matchedCallback) {
  return _removeAll.call(this, pathspec || "*", matchedCallback, null);
};

Index.prototype.updateAll = function(pathspec, matchedCallback) {
  return _updateAll.call(this, pathspec || "*", matchedCallback, null);
};
