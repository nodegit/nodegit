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
    throw new git.error('First parameter for Tree must be a raw repo');
  }

  if(!(rawTree instanceof git.raw.Tree)) {
    throw new git.error('Second parameter for Tree must be a raw tree');
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
  self.rawTree.entryByPath(path, function(error, rawEntry) {
    if (success(error, callback)) {
      callback(null, new git.entry(self.repo, rawEntry));
    }
  });
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
  blobsOnly = typeof blobsOnly === 'undefined' ? true : blobsOnly;

  var self = this,
      event = new events.EventEmitter(),
      entries = [];

  var total = 0;

  self.rawTree.walk(blobsOnly, function treeWalkEntries(error, rawEntries) {
    rawEntries.forEach(function treeWalkEntryEmitter(rawEntry) {
      var entry = new git.entry(self.repo, rawEntry);
      entries.push(entry);
      /**
       * Entry event.
       *
       * @event Tree#entry
       *
       * @param {GitError|null} error An error object if there was an issue, null otherwise.
       * @param {Entry} entry The tree entry.
       */
      event.emit('entry', null, entry);
    });
  }, function treeWalkEnd(error) {
    /**
     * End event.
     *
     * @event Tree#end
     *
     * @param {GitError|null} error An error object if there was an issue, null otherwise.
     * @param {Entry[]} entries The tree entries.
     */
      event.emit('end', error ? new git.error(error.message, error.code) : null, entries);
  });

  return event;
};

exports.tree = Tree;
