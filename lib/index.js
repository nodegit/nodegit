var git = require('../'),
    Index = git.Index;

/**
 * Return an array of the entries in this index.
 * @return {[IndexEntry]} an array of IndexEntrys
 */
Index.prototype.entries = function() {
  var size = this.size(),
      result = [];
  for (var i = 0; i < size; i++) {
    result.push(this.entry(i));
  }
  return result;
};
