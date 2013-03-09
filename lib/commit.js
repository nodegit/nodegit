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
 * @param  {RawCommit} obj
 * @return {Commit}
 */
var _Commit = function(obj) {
  var self = { _cache: {} };

  if(obj instanceof git.raw.Commit) {
    self.commit = obj;
  }
  else {
    self.commit = new git.raw.Commit();
  }

  /**
   * Fetch the commit's details asynchronously.
   *
   * @param  {Function} callback
   */
  self.fetchDetails = function(callback) {
    var error = null;
    self.commit.fetchDetails(function(errorCode, details) {
      if (errorCode) {
        error = git.error(errorCode);
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
   * @param  {Oid}   oid
   * @param  {Function} callback
   */
  self.lookup = function(repo, oid, callback) {
    self.repo = repo;
    self.commit.lookup(repo, oid, function(error, commit) {
      if (error) {
        return callback(error, null);
      }
      self.commit = commit;
      self.fetchDetails(function(error) {
       if (error) {
          return callback(error, null);
        }
        callback(null, self);
      });
    });
  };

  self.tree = function() {
    var tree = new git.raw.Tree( self.repo );
    if( tree.error ) {
      return git.error( tree.error );
    }
    else {
      self.commit.tree( tree );
    }

    return git.tree( self.repo, tree );
  };

  self.file = function(path) {
    return self.tree().entry( path );
  };

  self.history = function(start, end) {
    var revwalk = git.revwalk(self.repo),
        event = new events.EventEmitter(),
        commits = [];

    revwalk.walk(self.id, function(error, index, commit) {
      if(error) {
        if (error === git.error.GIT_EREVWALKOVER) {
          event.emit('end', null, commits);
          return;
        } else {
          event.emit('commit', new git.error(error), commit);
          return;
        }
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
