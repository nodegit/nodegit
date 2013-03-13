var git = require( '../' ),
  events = require( 'events' );

/**
 * Apply given details to the context.
 *
 * @param  {Object} details
 * @param  {Object} context
 * @return {Object} The modified context.
 */
function applyDetails(details, context) {
  if (details) {
    for (var detailKey in details) {
      context[detailKey] = details[detailKey];
    }
  }
  return context;
}

/**
 * Convenience commit constructor.
 *
 * @param  {RawCommit|Null} rawCommit
 * @return {Commit}
 */
var _Commit = function(rawCommit) {
  var self = {};

  if(rawCommit && rawCommit instanceof git.raw.Commit) {
    self.commit = rawCommit;
  } else {
    self.commit = new git.raw.Commit();
  }

  /**
   * Fetch the commit's details asynchronously.
   *
   * @param  {Function} callback
   */
  self.fetchDetails = function(callback) {
    var error = null;
    self.commit.fetchDetails(function(error, details) {
      if (error) {
        error = git.error(error);
      }
      applyDetails(details, self);
      callback(error);
    });
  };

  /**
   * Fetch the commit's details synchronously.
   */
  self.fetchDetailsSync = function(callback) {
    var details = self.commit.fetchDetailsSync();
    return applyDetails(details, self);
  };

  /**
   * Look up the commit referenced by oid, replace self.commit
   * with the result.
   *
   * @param  {Repo}   repo
   * @param  {Oid|String}   oid Raw OID object or SHA string
   * @param  {Function} callback
   */
  self.lookup = function(repo, oid, callback) {
    self.repo = repo;
    self.commit.lookup(repo, oid, function(error, commit) {
      if (error) {
        callback(git.error(error), null);
        return;
      }
      self.commit = commit;
      self.fetchDetails(function(error) {
        if (error) {
          callback(git.error(error), null);
          return;
        }
        callback(null, self);
      });
    });
  };

  self.tree = function() {
    var tree = new git.raw.Tree(self.repo);
    if(tree.error) {
      return git.error(tree.error);
    } else {
      self.commit.tree(tree);
    }
    return git.tree(self.repo, tree);
  };

  self.file = function(path) {
    return self.tree().entry(path);
  };

  /**
   * Walk the history of this commit.
   *
   * @return {Event} Event emits 'commit', with error, commit and 'end', with
   *                       error, commits[]
   */
  self.history = function() {
    var revwalk = git.revwalk(self.repo),
        event = new events.EventEmitter(),
        commits = [];

    revwalk.walk(self.id, function(error, index, commit, noMoreCommits) {
      if(error) {
        event.emit('end', error, commits);
        return false;
      }

      if (noMoreCommits) {
        event.emit('end', null, commits);
        return;
      }
      event.emit('commit', null, commit);
      commits.push(commit);
    });

    return event;
  };

  /**
   * Retrieve the commit's parent at the given position asynchronously.
   *
   * @param  {Integer} position
   */
  self.parent = function(position, callback) {
    var parent = null;
    self.commit.parent(position, function(errorCode, parent) {
      var error = null;
      if (errorCode) {
        error = git.error(errorCode);
        return callback(error, null);
      }
      var parentCommit = new _Commit(parent);
      parentCommit.fetchDetails(function(error) {
        callback(error, parentCommit);
      });
    });
  };

  /**
   * Retrieve the commit's parent at the given positino synchronously.
   *
   * @param  {Integer} position
   * @return {Commit}
   */
  self.parentSync = function(position) {
    var parent = new _Commit(self.commit.parentSync(position));
    return parent.fetchDetailsSync();
  };

  return self;
};

exports.commit = _Commit;
