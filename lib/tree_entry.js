var git = require('../'),
    path = require('path'),
    success = require('./utilities').success;

/**
 * Convenience tree entry constructor.
 *
 * @constructor
 * @param {git.Repo} repo Repo object.
 * @param {git.raw.TreeEntry} rawTreeEntry Raw tree entry object.
 */
var TreeEntry = function(repo, rawTreeEntry) {
  if(!(repo instanceof git.repo)) {
    throw new git.error('First parameter for Tree Entry must be a repo', 0);
  }

  if(!(rawTreeEntry instanceof git.raw.TreeEntry)) {
    throw new git.error('Second parameter for Tree Entry must be a raw tree entry', 0);
  }

  this.repo = repo;
  this.rawEntry = rawTreeEntry;
};

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

TreeEntry.prototype.isFile = function() {
  return this.filemode() === TreeEntry.FileMode.Blob ||
          this.filemode() === TreeEntry.FileMode.Executable;
};

TreeEntry.prototype.isDirectory = function() {
  return this.filemode() === TreeEntry.FileMode.Tree;
};

/**
 * Retrieve the Oid for this TreeEntry.
 */
TreeEntry.prototype.oid = function(callback) {
  return new git.oid(this.rawEntry.oid());
};

/**
 * Retrieve the SHA for this TreeEntry.
 */
TreeEntry.prototype.sha = function() {
  return this.oid().sha();
};

/**
 * Retrieve the SHA for this TreeEntry.
 */
TreeEntry.prototype.filemode = function() {
  return this.rawEntry.filemode();
};

/**
 * Retrieve the name for this TreeEntry.
 */
TreeEntry.prototype.name = function() {
  return this.rawEntry.name();
};

/**
 * Convert the TreeEntry to a blob.
 *
 * @param {TreeEntry~blobCallback} callback
 */
TreeEntry.prototype.getBlob = function(callback) {
  if (!this.isFile()) return callback(new git.error('Not a blob/file', 0));

  /**
   * @callback TreeEntry~blobCallback Callback executed after blob is retrieved.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Blob|null} blob the blob representation of the entry.
   */
  this.repo.blob(this.oid(), callback);
};

/**
 * Convert the TreeEntry to a tree.
 *
 * @param {TreeEntry~treeCallback} callback
 */
TreeEntry.prototype.getTree = function(callback) {
  if (!this.isDirectory()) return callback(new git.error('Not a tree/directory', 0));

  /**
   * @callback TreeEntry~treeCallback Callback executed after blob is retrieved.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Tree|null} blob the blob representation of the entry.
   */
  this.repo.tree(this.oid(), callback);
};

exports.entry = TreeEntry;
