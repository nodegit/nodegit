var git = require( '../' );

var _RevWalk = function( obj ) {
  var self = {};

  if( obj instanceof git.raw.Repo ) {
    self.repo = obj;
    self.revwalk = new git.raw.RevWalk( obj );
  }
  else if( obj instanceof git.raw.RevWalk ) {
    self.revwalk = obj;
  }

  /**
   * Walk the history from the given oid.
   *
   * @param  {Oid} oid
   * @param  {Function} callback Callback accepting the following arguments:
   *                             error, index, commit, noMoreCommits
   */
  self.walk = function(oid, callback ) {
    if(!callback) { return; }

    self.revwalk.push(oid);

    var shouldContinue = true, index = 0;

    function walk() {
      if(!shouldContinue) {
        return;
      }
      var _tmp = git.oid();

      self.revwalk.next( _tmp.oid, function(errorCode) {
        if(errorCode) {
          // Finished walking history, apply callback with noMoreCommits = true
          if (errorCode === git.error.GIT_EREVWALKOVER) {
            callback.apply(this, [null, index, null, true]);
            return;
          }
          callback.apply(this, [git.error(errorCode)]);
          return;
        }

        git.commit(self.repo).lookup(self.repo, _tmp.oid, function(errorCode, commit) {
          if(errorCode) {
            callback.apply(this, [git.error(errorCode), index, commit]);
            return;
          }

          if(callback.apply(this, [null, index, commit]) === false) {
            shouldContinue = false;
          }

          index++;
          walk();
        });
      });
    }

    walk();
  };

  return self;
};

exports.revwalk = _RevWalk;
