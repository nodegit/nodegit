var git = require("../");
var Tree = git.Tree;
var Treebuilder = git.Treebuilder;
var Diff = git.Diff;
var events = require("events");
var LookupWrapper = require("./util/lookupWrapper");

/**
* Retrieves the tree pointed to by the oid
* @param {Repository} repo The repo that the tree lives in
* @param {String|Oid|Tree} id The tree to lookup
* @param {Function} callback
* @return {Tree}
*/
Tree.lookup = LookupWrapper(Tree);

/**
 * Diff two trees
 * @param {Tree} tree to diff against
 * @param {Function} callback
 * @return {DiffList}
 */
Tree.prototype.diff = function(tree, callback) {
  return Diff.treeToTree(this.repo, tree, this, null).then(function(diff) {
    if (typeof callback === "function") {
      callback(null, diff);
    }

    return diff;
  }, callback);
};

/**
 * Get an entry at the ith position.
 *
 * @param {Number} i
 * @return {TreeEntry}
 */
Tree.prototype.entryByIndex = function(i) {
  var entry = this._entryByIndex(i);
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
Tree.prototype.getEntry = function(path, callback) {
  var tree = this;

  return this.entryByPath(path).then(function(entry) {
    entry.parent = tree;

    if (typeof callback === "function") {
      callback(null, entry);
    }

    return entry;
  });
};

/**
 * Return an array of the entries in this tree (excluding its children).
 * @return {[TreeEntry]} an array of TreeEntrys
 */
Tree.prototype.entries = function() {
  var size = this.entryCount();
  var result = [];

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

  var total = 1;

  // This looks like a DFS, but it is a BFS because of implicit queueing in
  // the recursive call to `entry.getTree(bfs)`
  function bfs(error, tree) {
    total--;

    if (error) {
      return event.emit("error", error);
    }
    var entries = tree.entries();
    entries.forEach(function (entry, entryIndex) {
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
 * @return {Treebuilder}
 */
Tree.prototype.builder = function() {
  var builder = Treebuilder.create(this);

  builder.root = builder;
  builder.repo = this.repo;

  return builder;
};

module.exports = Tree;
