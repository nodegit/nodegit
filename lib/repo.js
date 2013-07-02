var git = require('../'),
    util = require('./util.js'),
    Repo = git.Repo,
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

    self.getCommit(reference.oid(), function commitLookupCallback(error, commit) {
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
  oldGetCommit.call(this, normalizeOid(oid), function(error, commit) {
    if (error) return callback(error);
    commit.repo = self;
    callback(null, commit);
  });
};
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
  oldBlob.call(this, normalizeOid(oid), function(error, blob) {
    if (error) return callback(error);
    blob.repo = self;
    callback(null, blob);
  });
};
util.makeSafe(Repo.prototype, 'blob');

/**
 * Retrieve the blob represented by the oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Blob~lookupCallback} callback
 */
var oldGetTree = Repo.prototype.getTree;
Repo.prototype.getTree = function(oid, callback) {
  var self = this;
  oldGetTree.call(this, normalizeOid(oid), function(error, tree) {
    if (error) return callback(error);
    tree.repo = self;
    callback(null, tree);
  });
};
util.makeSafe(Repo.prototype, 'getTree');

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

function normalizeOid(oid) {
  if (typeof oid === 'string') oid = git.Oid.fromString(oid);
  return oid;
}
