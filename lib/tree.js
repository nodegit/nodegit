var git = require("../");
var Tree = git.Tree;
var TreeBuilder = git.Treebuilder;
var Diff = git.Diff;
var events = require("events");

var oldBuilder = TreeBuilder.create;
var oldEntryByIndex = Tree.prototype.entryByindex;

// Backwards compatibility.
Object.defineProperties(Tree.prototype, {
  "size": {
    value: Tree.prototype.entrycount,
    enumerable: false
  }
});

/**
 * Diff two trees
 * @param {Tree} tree to diff against
 * @param {Function} callback
 * @return {DiffList}
 */
Tree.prototype.diff = function(tree, callback) {
  return Diff.treeToTree(this.repo, tree, this, null).then(null, callback);
};

/**
 * Get an entry at the ith position.
 *
 * @param {Number} i
 * @return {TreeEntry}
 */
Tree.prototype.entryByIndex = function(i) {
  var entry = oldEntryByIndex.call(this, i);
  entry.parent = this;
  return entry;
};

/**
 * Get an entry by name; if the tree is a directory, the name is the filename.
 *
 * @param {String} name
 * @return {TreeEntry}
 */
Tree.prototype.entryByName = function(name) {
  var entry = this.entryByname(name);
  entry.parent = this;
  return entry;
};

/**
 * Get an entry at a path. Unlike by name, this takes a fully
 * qualified path, like `/foo/bar/baz.javascript`
 *
 * @param {String} path
 * @return {TreeEntry}
 */
Tree.prototype.getEntry = function(path) {
  // FIXME: this method ought to implement the recursion directly, rather than
  // rely on oldGetEntry, in order to ensure that `parent` pointers are direct.
  var self = this;

  return this.entryBypath(path).then(function(entry) {
    entry.parent = self;
    entry.path = function() { return path; };
    return entry;
  });
};

/**
 * Return an array of the entries in this tree (excluding its children).
 * @return {[TreeEntry]} an array of TreeEntrys
 */
Tree.prototype.entries = function() {
  var size = this.entrycount(),
      result = [];
  for (var i = 0; i < size; i++) {
    result.push(this.entryByIndex(i));
  }
  return result;
};

/**
 * Recursively walk the tree in breadth-first order. Fires an event for each
 * entry.
 *
 * @fires Tree#entry
 * @fires Tree#end
 *
 * @param {Boolean} [blobsOnly = true] True to emit only blob & blob executable
 * entries.
 *
 * @return {EventEmitter}
 */
Tree.prototype.walk = function(blobsOnly) {
  blobsOnly = typeof blobsOnly === "boolean" ? blobsOnly : true;

  var self = this;
  var event = new events.EventEmitter();
  var entries = [];

  var total = 1;

  // This looks like a DFS, but it is a BFS because of implicit queueing in
  // the recursive call to `entry.getTree(bfs)`
  function bfs(error, tree) {
    total--;

    if (error) {
      return event.emit("error", error);
    }

    tree.entries().forEach(function (entry) {
      if (!blobsOnly || entry.isFile()) {
        event.emit("entry", entry);
        entries.push(entry);
      }

      if (entry.isTree()) {
        total++;
        entry.getTree(bfs);
      }
    });

    if (total === 0) {
      event.emit("end", entries);
    }
  }

  event.start = function() {
    bfs(null, self);
  };

  return event;
};

/**
 * Return the path of this tree, like `/lib/foo/bar`
 * @return {String}
 */
Tree.prototype.path = function(blobsOnly) {
  return this.entry ? this.entry.path() : "";
};

/**
 * Make builder. This is helpful for modifying trees.
 * @return {TreeBuilder}
 */
Tree.prototype.builder = function() {
  var builder = oldBuilder.call(this);
  builder.root = builder;
  builder.repo = this.repo;
  return builder;
};

module.exports = Tree;
