var git = require('../'),
    fs = require('fs');

/* Module: Repo
 * Work with a repository.
 */
exports.repo = function(directory, callback) {
  var self = {
    // Assign a new repo object
    repo: new git.raw.Repo()
  };

  if (directory) {
    if (!callback || typeof callback !== 'function') {
      throw new Error('If directory is provided, callback function is required');
    }
    self.repo.open(directory, function(error, rawRepo) {
      if (error) {
        callback(git.error(error), null);
        return;
      }
      self.repo = rawRepo;
      callback(null, self);
    });
  }

  /**
   * Look up a branch and find its tree.
   *
   * @param  {String} name Branch name, e.g. 'master'
   * @param {Function}
   */
  self.branch = function(name, callback) {
    git.reference(self.repo).lookup('refs/heads/' + name, function referenceLookupCallback(error, reference) {
      if (error) {
        callback(git.error(error), null);
        return;
      }
      reference.oid(function oidCallback(error, oid) {
        if (error) {
          callback(git.error(error), null);
          return;
        }
        self.commit(oid, function commitLookupCallback(error, commit) {
          if (error) {
            callback(git.error(error), null);
            return;
          }
          callback(null, commit);
        });
      });
    });
  };

  // Find a single commit
  self.commit = function(oid, callback) {
    git.commit().lookup(self.repo, oid, function(error, commit) {
      callback(error, commit);
    });
  };

  self.commitSync = function(sha) {
    throw new Error('commitSync not yet implemented');
    // return git.commit().lookupSync(sha);
  };

  self.init = function(directory, isBare, callback) {
    self.repo.init(directory, isBare, function(error) {
      callback.call(this, error, self);
    });
  };

  self.free = function() {
    self.repo.free();
    delete self.repo;
  };

  return self;
};
