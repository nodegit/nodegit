var git = require( '../' ),
  events = require( 'events' );

/**
 * Convenience commit constructor.
 *
 * @param  {RawCommit} obj
 * @return {Commit}
 */
var _Commit = function(obj) {
  var self = { _cache: {} };

  if( obj instanceof git.raw.Commit ) {
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
      if (details) {
        // @todo add details to commit
      }
      callback(error);
    });
  };

  self.lookup = function(repo, oid, callback) {
    self.repo = repo;
    self.commit.lookup(repo, oid, function() {
      var args = Array.prototype.slice.call(arguments);
      args[0] = git.util().error(args[0]);
      self.fetchDetails(function(error) {
        callback.apply(self, args.concat(self));
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
    var parent =
    self.commit.parent(position, function(errorCode, parent) {
      var error = null;
      if (errorCode !== git.error.GIT_SUCCESS) {
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
    parent.fetchDetailsSync();
    return parent;
  };

  return self;
};

exports.commit = _Commit;
