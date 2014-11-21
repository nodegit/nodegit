var NodeGit = require("../");

var Index = NodeGit.Index;

/**
 * Return an array of the entries in this index.
 * @return {[IndexEntry]} an array of IndexEntrys
 */
Index.prototype.entries = function() {
  var size = this.entryCount();
  var result = [];

  for (var i = 0; i < size; i++) {
    result.push(this.getByIndex(i));
  }

  return result;
};

module.exports = Index;
