var git = require("../");

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
    self.repo.open(directory, function(error, repository) {
      if (error) {
        callback(git.error(error), null);
        return;
      }
      self.repo = repository;
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
    git.ref(self.repo).lookup('refs/heads/' + name, function(error, ref) {
      if (error) {
        callback(git.error(error), null);
        return;
      }

      git.commit(self.repo).lookup(self.repo, ref.oid().oid, function(error, commit) {
        if (error) {
          callback(git.error(error), null);
          return;
        }
        callback(null, commit);
      });
    });
  };

  // Find a single commit
  self.commit = function(sha, callback) {
    git.commit().lookup(self.repo, git.oid(sha).oid, callback);
  };

  self.commitSync = function(sha) {
    throw new Error('commitSync not yet implemented');
  };

  self.init = function(dir, isBare, async) {
    self.repo.init(dir, isBare, function() {
      git.util().asyncComplete.call(this, arguments, async);
    });

    return self;
  };

  self.free = function() {
    self.repo.free();
    delete self.repo;
  };

  return self;
};
