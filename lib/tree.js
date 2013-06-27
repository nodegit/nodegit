var git = require('../'),
    success = require('./utilities').success,
    events = require('events');

/**
 * Tree convenience class constructor.
 *
 * @constructor
 * @param {git.Repo} repo repository object.
 * @param {git.raw.Tree} [rawTree = new git.raw.Tree(rawRepo)] Raw tree object.
 */
var Tree = function(repo, rawTree) {
  if(!(repo instanceof git.repo)) {
    throw new Exception('First parameter for Tree must be a raw repo');
  }

  if(!(rawTree instanceof git.raw.Tree)) {
    throw new Exception('Second parameter for Tree must be a raw tree');
  }

  this.repo = repo;
  this.rawTree = rawTree;
};

/**
 * Retrieve the entry by path.
 *
 * @param  {String} path Path to the tree entry, relative to repository root.
 * @param  {Tree~entryCallback} callback
 */
Tree.prototype.entry = function(path, callback) {
  /**
   * @callback Tree~entryCallback Callback executed when an entry is retrieved.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Entry|null} entry The tree entry object or null.
   */
  var self = this;
  self.rawTree.getEntryByPath(path, function(error, rawEntry) {
    if (success(error, callback)) {
      callback(null, new git.entry(self.repo, rawEntry));
    }
  });
};

/**
 * Retrieve the number of entries in this tree.
 */
Tree.prototype.size = function() {
  return self.rawTree.size();
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

    var size = tree.rawTree.size();
    for (var i = 0; i < size; i ++) {
      var rawEntry = tree.rawTree.entryByIndex(i),
          entry = new git.entry(tree.repo, rawEntry);
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
    }
    if (total === 0)
      event.emit('end', errors.length ? errors : null, entries);
  }

  event.start = function() {
    dfs(null, self);
  };

  return event;
};

exports.tree = Tree;
