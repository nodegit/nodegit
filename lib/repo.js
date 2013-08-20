var git = require('../'),
    util = require('./util.js'),
    Repo = git.Repo,
    Tree = git.Tree,
    Reference = git.Reference;

/**
 * Look up a branch's most recent commit.
 *
 * @param {String} name Branch name, e.g. 'master'
 * @param {Repo~branchCallback} callback
 */
Repo.prototype.getBranch = function(name, callback) {
  /**
   * @callback Repo~branchCallback Callback executed when the branch is checked out.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Commit|null} repo HEAD commit for the branch.
   */
  var self = this;
  this.getReference('refs/heads/' + name, function referenceLookupCallback(error, reference) {
    if (error) return callback(error);

    self.getCommit(reference.target(), function commitLookupCallback(error, commit) {
      if (error) return callback(error);

      callback(null, commit);
    });
  });
};
util.makeSafe(Repo.prototype, 'getBranch');

/**
 * Lookup the reference with the given name.
 *
 * @param {String} name
 * @param {Reference~lookupCallback} callback
 */
var oldGetReference = Repo.prototype.getReference;
Repo.prototype.getReference = function(name, callback) {
  var self = this;
  oldGetReference.call(this, name, function(error, reference) {
    if (error) return callback(error);

    if (reference.type() == Reference.Type.Symbolic) {
      reference.resolve(function (error, reference) {
        if (error) return callback(error);
        reference.repo = self;
        callback(null, reference);
      });
    } else {
      reference.repo = self;
      callback(null, reference);
    }
  });
};
util.makeSafe(Repo.prototype, 'getReference');

/**
 * Retrieve the commit identified by oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Repo~commitCallback} callback
 */
var oldGetCommit = Repo.prototype.getCommit;
Repo.prototype.getCommit = function(oid, callback) {
  var self = this;
  oldGetCommit.call(this, oid, function(error, commit) {
    if (error) return callback(error);
    commit.repo = self;
    callback(null, commit);
  });
};
util.normalizeOid(Repo.prototype, 'getCommit');
util.makeSafe(Repo.prototype, 'getCommit');

/**
 * Retrieve the blob represented by the oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Blob~lookupCallback} callback
 */
var oldBlob = Repo.prototype.getBlob;
Repo.prototype.getBlob = function(oid, callback) {
  var self = this;
  oldBlob.call(this, oid, function(error, blob) {
    if (error) return callback(error);
    blob.repo = self;
    callback(null, blob);
  });
};
util.normalizeOid(Repo.prototype, 'getBlob');
util.makeSafe(Repo.prototype, 'getBlob');

/**
 * Retrieve the blob represented by the oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Blob~lookupCallback} callback
 */
var oldGetTree = Repo.prototype.getTree;
Repo.prototype.getTree = function(oid, callback) {
  var self = this;
  oldGetTree.call(this, oid, function(error, tree) {
    if (error) return callback(error);
    tree.repo = self;
    callback(null, tree);
  });
};
util.normalizeOid(Repo.prototype, 'getTree');
util.makeSafe(Repo.prototype, 'getTree');

/**
 * Retrieve the tag represented by the oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Blob~lookupCallback} callback
 */
var oldGetTag = Repo.prototype.getTag;
Repo.prototype.getTag = function(oid, callback) {
  var self = this;
  oldGetTag.call(this, oid, callback);
};
util.normalizeOid(Repo.prototype, 'getTag');
util.makeSafe(Repo.prototype, 'getTag');

/**
 * Retrieve the blob represented by the oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Blob~lookupCallback} callback
 */
var oldCreateRevWalk = Repo.prototype.createRevWalk;
Repo.prototype.createRevWalk = function() {
  var revWalk = oldCreateRevWalk.call(this);
  revWalk.repo = this;
  return revWalk;
};

/**
 * Retrieve the master branch.
 *
 * @param {Blob~lookupCallback} callback
 */
Repo.prototype.getMaster = function(callback) {
  this.getBranch('master', callback);
};

/**
 * Create a commit
 * @param {String} updateRef
 * @param {Signature} author
 * @param {Signature} commiter
 * @param {String} message
 * @param {Tree|Oid|String} Tree
 * @param {Array} parents
 * @param {Function} callback
 */
var oldCreateCommit = Repo.prototype.createCommit;
Repo.prototype.createCommit = function(updateRef, author, committer, message, tree, parents, callback) {
  if (tree instanceof Tree) {
    oldCreateCommit.call(
      this,
      updateRef,
      author,
      committer,
      null /* use default message encoding */,
      message,
      tree,
      parents.length, parents,
      callback);
  } else {
    var self = this;
    this.getTree(tree, function(error, tree) {
      if (error) return callback(error);
      oldCreateCommit.call(
        self,
        updateRef,
        author,
        committer,
        null /* use default message encoding */,
        message,
        tree,
        parents.length, parents,
        callback);
    });
  }
};

/**
 * Create a blob from a buffer
 *
 * @param {Buffer} buffer
 * @param {Function} callback
 */
var oldCreateBlobFromBuffer = Repo.prototype.createBlobFromBuffer;
Repo.prototype.createBlobFromBuffer = function(buffer, callback) {
  oldCreateBlobFromBuffer.call(this, buffer, buffer.length, callback);
};