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
  // @todo refactor to use git_tree_walk

  // var entry,
  //     index,
  //     length = self.length,
  //     event = new events.EventEmitter(),
  //     entries = [];

  // function next(index) {
  //   var dir;
  //   var tree;
  //   var prerequisites = 0;

  //   function complete(error) {
  //     if (index < length - 1) {
  //       next(index = index + 1);
  //     } else {
  //       event.emit('end', error, entries);
  //     }
  //   }

  //   entry = git.entry(repo);

  //   self.tree.entryByIndex(entry.entry, index, function() {
  //     if (entry.isFile()) {
  //       entries.push(entry);
  //       event.emit('entry', null, index, entry);
  //     } else {
  //       dir = entry.name;
  //       tree = entry.tree();
  //       prerequisites++;
  //       if (tree.error) {
  //         event.emit('end', tree.error);
  //         return;
  //       }

  //       tree.walk(repo).on('entry', function(error, index, entry) {
  //         if (error) {
  //           event.emit('entry', error, index, entry);
  //         }
  //         entry.dir = dir + '/' + entry.dir;
  //         event.emit('entry', null, index, entry);
  //       }).on('end', function(error, endEntries) {
  //         if (error) {
  //           complete(error);
  //         }
  //         prerequisites--;
  //         entries = entries.concat(endEntries);
  //         if (prerequisites === 0) {
  //           complete(error);
  //         }
  //       });
  //     }

  //     if (prerequisites === 0) {
  //       complete();
  //     }
  //   });
  // }

  // next(0);

  // return event;
};

exports.tree = Tree;
