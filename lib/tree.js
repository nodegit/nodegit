var path = require("path");
var events = require("events");
var NodeGit = require("../");
var Diff = NodeGit.Diff;
var LookupWrapper = NodeGit.Utils.lookupWrapper;
var Tree = NodeGit.Tree;
var Treebuilder = NodeGit.Treebuilder;

/**
 * Retrieves the tree pointed to by the oid.
 *
 * @param {Repository}          repo  The repo that the tree lives in.
 * @param {string | Oid | Tree} id    The tree to lookup.
 * @returns {Tree}
 * @async
 */
Tree.lookup = LookupWrapper(Tree);

/**
 * Make builder. This is helpful for modifying trees.
 *
 * @returns {Treebuilder}
 */
Tree.prototype.builder = function () {
  var builder = Treebuilder.create(this);

  builder.root = builder;
  builder.repo = this.repo;

  return builder;
};

/**
 * Diff two trees.
 *
 * @param {Tree} tree  To diff against.
 * @returns {Diff}
 * @async
 */
Tree.prototype.diff = function (tree) {
  return this.diffWithOptions(tree, null);
};

/**
 * Diff two trees with options.
 *
 * @param {Tree}   tree     To diff against.
 * @param {Object} options
 * @returns {Diff}
 * @async
 */
Tree.prototype.diffWithOptions = function (tree, options) {
  return Diff.treeToTree(this.repo, tree, this, options);
};

/**
 * Return an array of the entries in this tree (excluding its children).
 *
 * @returns {TreeEntry[]}
 */
Tree.prototype.entries = function () {
  var size = this.entryCount();
  var result = [];

  for (var i = 0; i < size; i++) {
    result.push(this.entryByIndex(i));
  }

  return result;
};

/**
 * Get an entry at the ith position.
 *
 * @param {number} i
 * @returns {TreeEntry}
 */
Tree.prototype.entryByIndex = function (i) {
  var entry = this._entryByIndex(i);
  entry.parent = this;
  return entry;
};

/**
 * Get an entry by name; if the tree is a directory, the name is the filename.
 *
 * @param {string} name
 * @returns {TreeEntry}
 */
Tree.prototype.entryByName = function (name) {
  var entry = this._entryByName(name);
  entry.parent = this;
  return entry;
};

/**
 * Get an entry at a path. Unlike by name, this takes a fully qualified path, like `/foo/bar/baz.javascript`
 *
 * @param {string} filePath
 * @returns {TreeEntry}
 * @async
 */
Tree.prototype.getEntry = function (filePath) {
  var tree = this;

  return this.entryByPath(filePath).then(function (entry) {
    entry.parent = tree;
    entry.dirtoparent = path.dirname(filePath);
    return entry;
  });
};

/**
 * Return the path of this tree, like `/lib/foo/bar`
 *
 * @returns {string}
 */
Tree.prototype.path = function (_blobsOnly) {
  return this.entry ? this.entry.path() : "";
};

/**
 * Recursively walk the tree in breadth-first order. Fires an event for each entry.
 *
 * @param {boolean} [blobsOnly=true]  True to emit only blob & blob executable entries.
 * @returns {EventEmitter}
 * @fires EventEmitter#entry  Tree.
 * @fires EventEmitter#end    Array<Tree>
 * @fires EventEmitter#error  Error.
 */
Tree.prototype.walk = function (blobsOnly) {
  blobsOnly = typeof blobsOnly === "boolean" ? blobsOnly : true;

  var self = this;
  var event = new events.EventEmitter();

  var total = 1;
  var entries = new Set();
  var finalEntires = [];

  // This looks like a DFS, but it is a BFS because of implicit queueing in
  // the recursive call to `entry.getTree(bfs)`
  function bfs(error, tree) {
    total--;

    if (error) {
      return event.emit("error", error);
    }

    tree.entries().forEach(function (entry, _entryIndex) {
      if (!blobsOnly || (entry.isFile() && !entries.has(entry))) {
        event.emit("entry", entry);
        entries.add(entry);

        // Node 0.12 doesn't support either [v for (v of entries)] nor
        // Array.from so we'll just maintain our own list.
        finalEntires.push(entry);
      }

      if (entry.isTree()) {
        total++;
        entry.getTree().then((result) => bfs(null, result), bfs);
      }
    });

    if (total === 0) {
      event.emit("end", finalEntires);
    }
  }

  event.start = function () {
    bfs(null, self);
  };

  return event;
};
