var path = require("path");
var NodeGit = require("../");
var TreeEntry = NodeGit.TreeEntry;

/**
 * Is this TreeEntry a blob? (i.e., a file)
 * @return {Boolean}
 */
TreeEntry.prototype.isFile = function() {
  return this.filemode() === TreeEntry.FILEMODE.BLOB ||
         this.filemode() === TreeEntry.FILEMODE.EXECUTABLE;
};

/**
 * Is this TreeEntry a tree? (i.e., a directory)
 * @return {Boolean}
 */
TreeEntry.prototype.isTree = function() {
  return this.filemode() === TreeEntry.FILEMODE.TREE;
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
  return this.id().toString();
};

/**
 * Retrieve the tree for this entry. Make sure to call `isTree` first!
 * @async
 * @return {Tree}
 */
TreeEntry.prototype.getTree = function(callback) {
  var entry = this;

  return this.parent.repo.getTree(this.id()).then(function(tree) {
    tree.entry = entry;

    if (typeof callback === "function") {
      callback(null, tree);
    }

    return tree;
  }, callback);
};

/**
 * Retrieve the blob for this entry. Make sure to call `isBlob` first!
 * @async
 * @return {Blob}
 */
TreeEntry.prototype.getBlob = function(callback) {
  return this.parent.repo.getBlob(this.id()).then(function(blob) {
    if (typeof callback === "function") {
      callback(null, blob);
    }

    return blob;
  }, callback);
};

/**
 * Returns the path for this entry.
 * @return {String}
 */
TreeEntry.prototype.path = function(callback) {
  var dirtoparent = this.dirtoparent || "";
  return path.join(this.parent.path(), dirtoparent, this.name());
};

/**
 * Alias for `path`
 */
TreeEntry.prototype.toString = function() {
  return this.path();
};

TreeEntry.prototype.oid = function() {
  return this.id().toString();
};
