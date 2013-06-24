var git = require('../'),
    success = require('./utilities').success;

/**
 * Convenience repository class.
 *
 * @constructor
 */
var Repo = function(rawRepo) {
  this.rawRepo = rawRepo;
};

/**
 * Initialise a git repository at directory.
 *
 * @param {String} directory
 * @param {Boolean} isBare True if the repository is to be bare, false otherwise.
 * @param {Repo~initCallback} callback
 */
Repo.init = function(directory, isBare, callback) {
  /**
   * @callback Repo~initCallback Callback executed when repository is initialized.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Repo|null} repo Initialized repository.
   */
  git.raw.Repo.init(directory, isBare, function(error, rawRepo) {
    if (success(error, callback)) {
      callback(null, new Repo(rawRepo));
    }
  });
};

/**
 * Open the git repository at directory.
 *
 * @example
 * git.repo('/path/to/repository/.git', function(error, repo) { });
 *
 * @param {String} directory The .git directory for the repository to open.
 * @param {Repo~openCallback} callback
 */
Repo.open = function(directory, callback) {
  /**
   * @callback Repo~openCallback Callback executed when repository is opened.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Repo|null} repo Opened repository.
   */
  if (typeof callback !== 'function') {
    throw new git.error('Callback is required and must be a Function');
  }
  git.raw.Repo.open(directory, function openRepository(error, rawRepo) {
    if (success(error, callback)) {
      callback(null, new Repo(rawRepo));
    }
  });
};

/**
 * Look up a branch's most recent commit.
 *
 * @param {String} name Branch name, e.g. 'master'
 * @param {Repo~branchCallback} callback
 */
Repo.prototype.branch = function(name, callback) {
  /**
   * @callback Repo~branchCallback Callback executed when the branch is checked out.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Commit|null} repo HEAD commit for the branch.
   */
  var self = this;
  git.reference.lookup(this.rawRepo, 'refs/heads/' + name, function referenceLookupCallback(error, reference) {
    if (!success(error, callback)) return;

    var oid = reference.oid();
    self.commit(oid, function commitLookupCallback(error, commit) {
      if (!success(error, callback)) return;

      callback(null, commit);
    });
  });
};

/**
 * Retrieve the commit identified by oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Repo~commitCallback} callback
 */
Repo.prototype.commit = function(oid, callback) {
    /**
   * @callback Repo~commitCallback Callback executed when the commit is looked up.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Commit|null} commit Commit represented by sha.
   */
  var self = this;
  try {
    if (typeof oid === 'string') oid = git.raw.Oid.fromString(oid);

    git.raw.Commit.lookup(this.rawRepo, oid, function(error, rawCommit) {
      if (success(error, callback)) {
        callback(null, new git.commit(self, rawCommit));
      }
    });
  } catch (e) {
    callback(e);
  }
};

/**
 * Retrieve the blob represented by the oid.
 *
 * @param  {git.Oid} oid The OID representing the blob to lookup.
 * @param {Blob~lookupCallback} callback
 */
Repo.prototype.blob = function(oid, callback) {
  /**
   * @callback Blob~lookupCallback Callback executed on lookup completion.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Blob|null} blob Retrieved blob object or null.
   */
  git.raw.Blob.lookup(this.rawRepo, oid.rawOid, function blobLookup(error, rawBlob) {
    if (success(error, callback)) {
      callback(null, new git.blob(rawBlob));
    }
  });
};

/**
 * Retrieve the raw tree identified by the given Oid.
 *
 * @param  {Oid} oid The Oid identifying a tree.
 * @param  {Tree~lookupCallback} callback
 */
Repo.prototype.tree = function(oid, callback) {
  /**
   * @callback Tree~lookupCallback Callback executed when the tree is retrieved.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Tree|null} tree The tree object or null.
   */
  var self = this;
  git.raw.Tree.lookup(this.rawRepo, oid.rawOid, function(error, rawTree) {
    if (success(error, callback)) {
      callback(null, new git.tree(self, rawTree));
    }
  });
};

exports.repo = Repo;
