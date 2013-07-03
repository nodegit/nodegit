var git = require('../'),
    RevWalk = git.RevWalk;

/**
 * Refer to vendor/libgit2/include/git2/revwalk.h for sort definitions.
 */
RevWalk.Sort = {
  None: 0,
  Topological: 1,
  Time: 2,
  Reverse: 4
};

/**
 * Set the sort order for the revwalk
 */
var oldSorting = RevWalk.prototype.sorting;
RevWalk.prototype.sorting = function() {
  var sort = 0;
  for (var i = 0; i < arguments.length; i++)
    sort |= arguments[i];
  oldSorting.call(this, sort);
};

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
