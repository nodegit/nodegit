var path = require("path");
var NodeGit = require("../");

var Tree = NodeGit.Tree;
var TreeEntry = NodeGit.TreeEntry;

/**
 * Refer to vendor/libgit2/include/git2/types.h for filemode definitions.
 *
 * @readonly
 * @enum {Integer}
 */
TreeEntry.FileMode = {
  /** 0000000 */ New: 0,
  /** 0040000 */ Tree: 16384,
  /** 0100644 */ Blob: 33188,
  /** 0100755 */ Executable: 33261,
  /** 0120000 */ Link: 40960,
  /** 0160000 */ Commit: 57344
};

/**
 * Is this TreeEntry a blob? (i.e., a file)
 * @return {Boolean}
 */
TreeEntry.prototype.isFile = function() {
  return this.attr() === TreeEntry.FileMode.Blob ||
         this.attr() === TreeEntry.FileMode.Executable;
};

/**
 * Is this TreeEntry a tree? (i.e., a directory)
 * @return {Boolean}
 */
TreeEntry.prototype.isTree = function() {
  return this.attr() === TreeEntry.FileMode.Tree;
};

/**
 * Is this TreeEntry a directory? Alias for `isTree`
 * @return {Boolean}
 */
TreeEntry.prototype.isDirectory = TreeEntry.prototype.isTree;

/**
 * Is this TreeEntry a blob? Alias for `isFile`
 * @return {Boolean}
 */
TreeEntry.prototype.isBlob = TreeEntry.prototype.isFile;

/**
 * Retrieve the SHA for this TreeEntry.
 * @return {String}
 */
TreeEntry.prototype.sha = function() {
  return this.oid().toString();
};

/**
 * Retrieve the tree for this entry. Make sure to call `isTree` first!
 * @return {Tree}
 */
TreeEntry.prototype.getTree = function(callback) {
  var entry = this;

  return this.parent.repo.getTree(this.oid()).then(function(tree) {
    tree.entry = entry;

    if (typeof callback === "function") {
      callback(null, tree);
    }

    return tree;
  }, callback);
};

/**
 * Retrieve the tree for this entry. Make sure to call `isTree` first!
 * @return {Blob}
 */
TreeEntry.prototype.getBlob = function() {
  return this.parent.repo.getBlob(this.oid());
};

/**
 * Returns the path for this entry.
 * @return {String}
 */
TreeEntry.prototype.path = function(callback) {
  return path.join(this.parent.path(), this.name());
};

/**
 * Alias for `path`
 */
TreeEntry.prototype.toString = function() {
  return this.path();
};

TreeEntry.prototype.oid = function() {
  return Tree.entryId(this).toString();
};

module.exports = TreeEntry;
