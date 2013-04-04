var git = require('../'),
    success = require('./utilities').success;

/**
 * Convenience repository class.
 *
 * @constructor
 */
var Repo = function() {
  this.rawRepo = new git.raw.Repo();
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
Repo.prototype.open = function(directory, callback) {
  /**
   * @callback Repo~openCallback Callback executed when repository is opened.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Repo|null} repo Opened repository.
   */
  if (typeof callback !== 'function') {
    throw new git.error('Callback is required and must be a Function');
  }
  var self = this;
  self.rawRepo.open(directory, function openRepository(error, rawRepo) {
    if (success(error, callback)) {
      self.rawRepo = rawRepo;
      callback(null, self);
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
  (new git.reference(self.rawRepo)).lookup('refs/heads/' + name, function referenceLookupCallback(error, reference) {
    if (!success(error, callback)) {
      return;
    }
    reference.oid(function oidCallback(error, oid) {
      if (!success(error, callback)) {
        return;
      }
      self.commit(oid, function commitLookupCallback(error, commit) {
        if (!success(error, callback)) {
          return;
        }
        callback(null, commit);
      });
    });
  });
};

/**
 * Retrieve the commit identified by oid.
 *
 * @param {String|Oid|git.raw.Oid} sha
 * @param {Repo~commitCallback} callback
 */
Repo.prototype.commit = function(sha, callback) {
    /**
   * @callback Repo~commitCallback Callback executed when the commit is looked up.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Commit|null} commit Commit represented by sha.
   */
  (new git.commit(this.rawRepo)).lookup(sha, function(error, commit) {
    if (success(error, callback)) {
      callback(null, commit);
    }
  });
};

/**
 * Initialise a git repository at directory.
 *
 * @param {String} directory
 * @param {Boolean} isBare True if the repository is to be bare, false otherwise.
 * @param {Repo~initCallback} callback
 */
Repo.prototype.init = function(directory, isBare, callback) {
  /**
   * @callback Repo~initCallback Callback executed when repository is initialized.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Repo|null} repo Initialized repository.
   */
  var self = this;
  self.rawRepo.init(directory, isBare, function(error, rawRepo) {
    if (success(error, callback)) {
      self.rawRepo = rawRepo;
      callback(null, self);
    }
  });
};

/**
 * Create a new Repo object. If directory is not provided, simply return it.
 * Otherwise open the repo asynchronously.
 *
 * @param {String|null} directory The directory for the git repo to open. Null
 *                                if one does not want to open a repo.
 * @param {repoCallback|null} callback
 * @return {Repo|null} The Repo if no directory is provided, else undefined.
 */
exports.repo = function(directory, callback) {
  /**
   * @callback repoCallback Callback executed if repository is opened.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Repo|null} repo Opened repository.
   */
  var repo = new Repo();
  if (typeof directory === 'undefined') {
    return repo;
  }
  repo.open(directory, callback);
};
