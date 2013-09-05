var git = require('../'),
    TreeEntry = git.TreeEntry,
    path = require('path');

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
  return this.filemode() === TreeEntry.FileMode.Blob ||
         this.filemode() === TreeEntry.FileMode.Executable;
};

/**
 * Is this TreeEntry a tree? (i.e., a directory)
 * @return {Boolean}
 */
TreeEntry.prototype.isTree = function() {
  return this.filemode() === TreeEntry.FileMode.Tree;
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
  return this.oid().sha();
};

/**
 * Retrieve the tree for this entry. Make sure to call `isTree` first!
 * @return {Tree}
 */
TreeEntry.prototype.getTree = function(callback) {
  var self = this;
  this.parent.repo.getTree(this.oid(), function(error, tree) {
    if (error) return callback(error);

    tree.entry = self;
    callback(null, tree);
  });
};

/**
 * Retrieve the tree for this entry. Make sure to call `isTree` first!
 * @return {Blob}
 */
TreeEntry.prototype.getBlob = function(callback) {
  this.parent.repo.getBlob(this.oid(), callback);
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
