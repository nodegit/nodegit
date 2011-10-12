var git = require("../");

/* Module: Repo
 * Work with a repository.
 */
exports.repo = function(dir, async) {
  var self = {
    // Assign a new repo object
    repo: new git.raw.Repo()
  };

  if (dir && async) {
    self.repo.open(dir, function() {
      git.util().asyncComplete.call(this, arguments, async);
    });
  }
  else if (dir) {
    // TODO: Make this eventually
    // this.repo.openSync(path.resolve(dir)
    //this.repo.open(path.resolve(dir)
    self.repo.open(dir);
  }

  // Look up a branch and find its tree
  self.branch = function(name, async) {
    if (!async) {
      // TODO: Implement Sync API
      return;
    }

    git.ref(self.repo).lookup("refs/heads/" + name, function(err, ref) {
      if (err) {
        return git.util().asyncComplete.call(this, arguments, async);
      }

      git.commit(self.repo).lookup(self.repo, ref.oid().oid, function() {
        git.util().asyncComplete.call(this, arguments, async);
      });
    });
  };

  // Find a single commit
  self.commit = function(sha, async) {
    if (!async) {
      // TODO: Implement Sync API
      return;
    }

    git.commit().lookup(self.repo, git.oid(sha).oid, async);
  };

  self.init = function(dir, isBare, async) {
    if (!async) {
      // TODO: Implement Sync API
      return;
    }

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
