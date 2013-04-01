var git = require('../'),
    success = require('./utilities').success,
    events = require('events');

/**
 * Tree convenience class constructor.
 *
 * @param {git.raw.Repo} rawRepo
 * @param {git.raw.Tree|null} rawTree
 */
var Tree = function(rawRepo, rawTree) {
  if(!(rawRepo instanceof git.raw.Repo)) {
    throw git.error('First parameter for Tree must be a raw repo');
  }
  this.rawRepo = rawRepo;

  if(rawTree instanceof git.raw.Tree) {
    this.rawTree = rawTree;
  } else {
    this.rawTree = new git.raw.Tree(this.rawRepo);
  }
};

/**
 * Retrieve the entry by path.
 *
 * @param  {String}   path
 * @param  {Function} callback
 */
Tree.prototype.entry = function(path, callback) {
  var self = this;
  self.rawTree.entryByPath(path, function(error, rawEntry) {
    if (success(error, callback)) {
      callback(null, new git.entry(self.rawRepo, rawEntry));
    }
  });
};

Tree.prototype.walk = function() {
  var self = this,
      event = new events.EventEmitter(),
      entries = [];

  self.rawTree.walk(function treeWalkEntries(error, rawEntries) {
    rawEntries.forEach(function treeWalkEntryEmitter(rawEntry) {
      var entry = new git.entry(self.rawRepo, rawEntry);
      entries.push(entry);
      if (!error) {
        event.emit('entry', null, entry);
      }
    });
  }, function treeWalkEnd(error) {
    if (error) {
      event.emit('end', git.error(error), entries);
    }
    event.emit('end', null, entries);
  });

  return event;
};

exports.tree = Tree;
