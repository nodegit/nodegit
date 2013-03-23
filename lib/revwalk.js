var git = require( '../' );

var _RevWalk = function( obj ) {
  var self = {};

  if (obj instanceof git.raw.Repo) {
    self.repo = obj;
    self.revwalk = new git.raw.RevWalk(obj);
  } else if (obj instanceof git.raw.RevWalk) {
    self.revwalk = obj;
  }

  /**
   * Walk the history from the given oid.
   *
   * @param  {Oid} oid
   * @param  {Function} callback Callback accepting the following arguments:
   *                             error, index, commit, noMoreCommits
   */
  self.walk = function(oid, callback) {
    if(!callback) { return; }

    self.revwalk.push(oid.getRawOid(), function(error) {
      if (error) {
        callback.apply(this, [git.error(error)]);
        return;
      }

      var shouldContinue = true, index = 0;

      function walk() {
        if(!shouldContinue) {
          return;
        }

        self.revwalk.next(function(error, oid, walkOver) {
          if(error) {
            callback.apply(this, [git.error(error), index, commit]);
            return;
          }

          // Finished walking history, apply callback with noMoreCommits = true
          if (walkOver) {
            callback.apply(this, [null, index, null, walkOver]);
            return;
          }

          git.commit(self.repo).lookup(self.repo, oid, function(error, commit) {

            if(error) {
              callback.apply(this, [git.error(error), index, commit]);
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
    });
  };

  return self;
};

exports.revwalk = _RevWalk;
