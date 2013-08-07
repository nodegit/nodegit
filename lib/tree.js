var git = require('../'),
    Tree = git.Tree,
    events = require('events'),
    path = require('path');

/**
 * Diff two trees
 */
Tree.prototype.diff = function(that, callback) {
  this.diffTree(this.repo, that, null, callback);
};

/**
 * Get an entry at the ith position.
 *
 * @param {Number} i
 */
var oldEntryByIndex = Tree.prototype.entryByIndex;
Tree.prototype.entryByIndex = function(i) {
  var entry = oldEntryByIndex.call(this, i);
  entry.parent = this;
  return entry;
};

/**
 * Get an entry by name:
 *
 * @param {String} name
 */
var oldEntryByName = Tree.prototype.entryByName;
Tree.prototype.entryByName = function(name) {
  var entry = oldEntryByName.call(this, name);
  entry.parent = this;
  return entry;
};

/**
 * Get an entry at a path
 *
 * @param {String} path
 */
var oldGetEntry = Tree.prototype.getEntry;
Tree.prototype.getEntry = function(path, callback) {
  // FIXME: this method ought to implement the recursion directly, rather than
  // rely on oldGetEntry, in order to ensure that `parent` pointers are direct.
  var self = this;
  oldGetEntry.call(this, path, function(error, entry) {
    if (error) return callback(error);

    // This is unlikely to be the *direct* parent; some methods, like `path()`
    // rely on directness.
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

  // This looks like a DFS, but it is a BFS because of implicit queueing in
  // the recursive call to `entry.getTree(bfs)`
  function bfs(error, tree) {
    total--;
    if (error) return errors.push(error);

    tree.entries().forEach(function (entry) {
      if (!blobsOnly || entry.isFile()) {
        /**
         * Entry event.
         *
         * @event Tree#entry
         *
         * @param {Entry} entry The tree entry.
         */
        event.emit('entry', entry);
        entries.push(entry);
      }

      if (entry.isTree()) {
        total++;
        entry.getTree(bfs);
      }
    });
    if (total === 0)
      event.emit('end', errors.length ? errors : null, entries);
  }

  event.start = function() {
    bfs(null, self);
  };

  return event;
};

/**
 * Return the path of this tree.
 */
Tree.prototype.path = function(blobsOnly) {
  return this.entry ? this.entry.path() : '';
};

/**
 * Make builder
 */
var oldBuilder = Tree.prototype.builder;
Tree.prototype.builder = function() {
  var builder = oldBuilder.call(this);
  builder.root = builder;
  builder.repo = this.repo;
  return builder;
};
