var path = require("path").posix;
var NodeGit = require("../");
var TreeEntry = NodeGit.TreeEntry;

/**
 * Retrieve the blob for this entry. Make sure to call `isBlob` first!
 *
 * @returns {Blob}
 * @async
 */
TreeEntry.prototype.getBlob = function () {
  return this.parent.repo.getBlob(this.id());
};

/**
 * Retrieve the tree for this entry. Make sure to call `isTree` first!
 *
 * @returns {Tree}
 * @async
 */
TreeEntry.prototype.getTree = function () {
  var entry = this;

  return this.parent.repo.getTree(this.id()).then(function (tree) {
    tree.entry = entry;
    return tree;
  });
};

/**
 * Is this TreeEntry a blob? Alias for `isFile`
 *
 * @returns {boolean}
 */
TreeEntry.prototype.isBlob = function () {
  return this.isFile();
};

/**
 * Is this TreeEntry a directory? Alias for `isTree`
 *
 * @returns {boolean}
 */
TreeEntry.prototype.isDirectory = function () {
  return this.isTree();
};

/**
 * Is this TreeEntry a blob? (i.e., a file)
 *
 * @returns {boolean}
 */
TreeEntry.prototype.isFile = function () {
  return this.filemode() === TreeEntry.FILEMODE.BLOB || this.filemode() === TreeEntry.FILEMODE.EXECUTABLE;
};

/**
 * Is this TreeEntry a submodule?
 *
 * @returns {boolean}
 */
TreeEntry.prototype.isSubmodule = function () {
  return this.filemode() === TreeEntry.FILEMODE.COMMIT;
};

/**
 * Is this TreeEntry a tree? (i.e., a directory)
 *
 * @returns {boolean}
 */
TreeEntry.prototype.isTree = function () {
  return this.filemode() === TreeEntry.FILEMODE.TREE;
};

/**
 * Retrieve the SHA for this TreeEntry. Alias for `sha`
 *
 * @returns {string}
 */
TreeEntry.prototype.oid = function () {
  return this.sha();
};

/**
 * Returns the path for this entry.
 *
 * @returns {string}
 */
TreeEntry.prototype.path = function () {
  var dirtoparent = this.dirtoparent || "";
  return path.join(this.parent.path(), dirtoparent, this.name());
};

/**
 * Retrieve the SHA for this TreeEntry.
 *
 * @returns {string}
 */
TreeEntry.prototype.sha = function () {
  return this.id().toString();
};

/**
 * Alias for `path`
 */
TreeEntry.prototype.toString = function () {
  return this.path();
};
