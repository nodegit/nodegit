var path = require("path").posix;
var NodeGit = require("../");
var TreeEntry = NodeGit.TreeEntry;

/**
 * Retrieve the blob for this entry. Make sure to call `isBlob` first!
 * @async
 * @return {Blob}
 */
TreeEntry.prototype.getBlob = function() {
  return this.parent.repo.getBlob(this.id());
};

/**
 * Retrieve the tree for this entry. Make sure to call `isTree` first!
 * @async
 * @return {Tree}
 */
TreeEntry.prototype.getTree = function() {
  var entry = this;

  return this.parent.repo.getTree(this.id()).then(function(tree) {
    tree.entry = entry;
    return tree;
  });
};

/**
 * Is this TreeEntry a blob? Alias for `isFile`
 * @return {Boolean}
 */
TreeEntry.prototype.isBlob = function() {
  return this.isFile();
};

/**
 * Is this TreeEntry a directory? Alias for `isTree`
 * @return {Boolean}
 */
TreeEntry.prototype.isDirectory = function() {
  return this.isTree();
};

/**
 * Is this TreeEntry a blob? (i.e., a file)
 * @return {Boolean}
 */
TreeEntry.prototype.isFile = function() {
  return this.filemode() === TreeEntry.FILEMODE.BLOB ||
         this.filemode() === TreeEntry.FILEMODE.EXECUTABLE;
};

/**
 * Is this TreeEntry a submodule?
 * @return {Boolean}
 */
TreeEntry.prototype.isSubmodule = function() {
  return this.filemode() === TreeEntry.FILEMODE.COMMIT;
};

/**
 * Is this TreeEntry a tree? (i.e., a directory)
 * @return {Boolean}
 */
TreeEntry.prototype.isTree = function() {
  return this.filemode() === TreeEntry.FILEMODE.TREE;
};

/**
 * Retrieve the SHA for this TreeEntry. Alias for `sha`
 * @return {String}
 */
TreeEntry.prototype.oid = function() {
  return this.sha();
};

/**
 * Returns the path for this entry.
 * @return {String}
 */
TreeEntry.prototype.path = function() {
  var dirtoparent = this.dirtoparent || "";
  return path.join(this.parent.path(), dirtoparent, this.name());
};

/**
 * Retrieve the SHA for this TreeEntry.
 * @return {String}
 */
TreeEntry.prototype.sha = function() {
  return this.id().toString();
};

/**
 * Alias for `path`
 */
TreeEntry.prototype.toString = function() {
  return this.path();
};
