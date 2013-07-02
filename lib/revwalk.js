var git = require('../'),
    RevWalk = git.RevWalk;

/**
 * Walk the history from the given oid.
 *
 * @param  {Oid} oid
 * @param  {Function} callback Callback accepting the following arguments:
 *                             error, index, commit, noMoreCommits
 */
RevWalk.prototype.walk = function(oid, callback) {
  var self = this;
  this.push(oid, function revWalkPush(error) {
    if (error) return callback(error);

    function walk() {
      self.next(function revWalkNext(error, oid) {
        if (error) return callback(new git.error(error.message, error.code));
        if (!oid) return callback();

        self.repo.getCommit(oid, function revWalkCommitLookup(error, commit) {
          if (error) return callback(new git.error(error.message, error.code));

          callback(null, commit);
          walk();
        });
      });
    }
    walk();
  });
};
