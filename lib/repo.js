var git = require('../'),
    success = require('./utilities').success;

var Repo = function() {
  this.rawRepo = new git.raw.Repo();
};

/**
 * Open the git repository at directory.
 *
 * @param  {String}   directory
 * @param  {Function} callback
 */
Repo.prototype.open = function(directory, callback) {
   var self = this;
   self.rawRepo.open(directory, function(error, rawRepo) {
    if (success(error, callback)) {
      self.rawRepo = rawRepo;
      callback(null, self);
    }
  });
};

/**
 * Look up a branch's most recent commit.
 *
 * @param  {String} name Branch name, e.g. 'master'
 * @param {Function} callback
 */
Repo.prototype.branch = function(name, callback) {
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
 * Retrieve the commit identified buy oid.
 * @param  {String|git.oid|git.raw.Oid} oid
 * @param  {Function} callback
 */
Repo.prototype.commit = function(sha, callback) {
  (new git.commit(this.rawRepo)).lookup(sha, function(error, commit) {
    if (success(error, callback)) {
      callback(null, commit);
    }
  });
};

/**
 * Initialise a git repository at directory.
 *
 * @param  {String}   directory
 * @param  {Boolean}  isBare True if the repository is to be bare, false otherwise.
 * @param  {Function} callback
 */
Repo.prototype.init = function(directory, isBare, callback) {
  var self = this;
  self.repo.init(directory, isBare, function(error, rawRepo) {
    if (success(error, callback)) {
      self.rawRepo = rawRepo;
      callback(null, self);
    }
  });
};

/**
 * @todo necessary?
 */
Repo.prototype.free = function() {
  this.rawRepo.free();
  delete self.repo;
};

/**
 * Create a new Repo object. If directory is not provided, simply return it.
 * Otherwise open the repo asynchronously.
 *
 * @param  {String|null}   directory The directory for the git repo to open. Null
 *                                   if one does not want to open a repo.
 * @param  {Function|null} callback
 * @return {Repo|null} The Repo if no directory is provided, else undefined.
 */
exports.repo = function(directory, callback) {
  var repo = new Repo();
  if (typeof directory === 'undefined') {
    return repo;
  }
  repo.open(directory, callback);
};
