var git = require('../'),
    Tree = git.Tree,
    events = require('events');

/**
 * Diff two trees
 */
Tree.prototype.diff = function(that, callback) {
  this.diffTree(this.repo, that, null, callback);
};

/**
 * Get an entry at the ith position.
 */
var oldEntryByIndex = Tree.prototype.entryByIndex;
Tree.prototype.entryByIndex = function(i) {
  var entry = oldEntryByIndex.call(this, i);
  entry.parent = this;
  return entry;
};

/**
 * Get an entry at the ith position.
 */
Tree.prototype.entry = Tree.prototype.entryByIndex;

/**
 * Get an entry at the ith position.
 */
var oldGetEntryByPath = Tree.prototype.getEntryByPath;
Tree.prototype.getEntryByPath = function(path, callback) {
  var self = this;
  oldGetEntryByPath.call(this, path, function(error, entry) {
    if (error) return callback(error);

    entry.parent = self;
    callback(null, entry);
  });
};

/**
 * Return an array of the entries in this tree (excluding its children).
 */
Tree.prototype.entries = function() {
  var size = this.size(),
      result = [];
  for (var i = 0; i < size; i++) {
    result.push(this.entryByIndex(i));
  }
  return result;
};

/**
 * Walk the tree.
 *
 * @fires Tree#entry
 * @fires Tree#end
 *
 * @param {Boolean} [blobsOnly = true] True to emit only blob & blob executable entries.
 *
 * @return {EventEmitter}
 */
Tree.prototype.walk = function(blobsOnly) {
  if (typeof blobsOnly == 'undefined') blobsOnly = true;

  var self = this,
      event = new events.EventEmitter(),
      entries = [],
      errors = [];

  var total = 1;

  function dfs(error, tree) {
    total--;
    if (error) return errors.push(error);

    tree.entries().forEach(function (entry) {
      if (!blobsOnly || entry.isFile()) {
        /**
         * Entry event.
         *
         * @event Tree#entry
         *
         * @param {GitError|null} error An error object if there was an issue, null otherwise.
         * @param {Entry} entry The tree entry.
         */
        event.emit('entry', entry);
        entries.push(entry);
      }

      if (entry.isTree()) {
        total++;
        entry.getTree(dfs);
      }
    });
    if (total === 0)
      event.emit('end', errors.length ? errors : null, entries);
  }

  event.start = function() {
    dfs(null, self);
  };

  return event;
};
