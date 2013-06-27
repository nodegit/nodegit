var git = require('../');

/**
 * Convenience revision walking class
 *
 * @constructor
 * @param {git.repo} repo
 * @param {git.raw.RevWalk|null} rawRevWalk
 */
var RevWalk = function(repo, rawRevWalk) {
  if (!(repo instanceof git.repo)) {
    throw new Error('First parameter for RevWalk must be a repo');
  }

  if (!(rawRevWalk instanceof git.raw.RevWalk)) {
    throw new Error('Second parameter for RevWalk must be a raw.RevWalk');
  }

  this.repo = repo;
  this.rawRevWalk = rawRevWalk;
};

/**
 * Make a new revwalker.
 */
RevWalk.make = function(repo) {
  return new RevWalk(repo, git.raw.RevWalk.make(repo.rawRepo));
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
  this.rawRevWalk.push(oid.getRawOid(), function revWalkPush(error) {
    if (error) return callback(error);

    function walk() {
      self.rawRevWalk.next(function revWalkNext(error, oid) {
        if (error) return callback(new git.error(error.message, error.code));
        if (!oid) return callback();

        self.repo.commit(oid, function revWalkCommitLookup(error, commit) {
          if (error) return callback(new git.error(error.message, error.code));

          callback(null, commit);
          walk();
        });
      });
    }
    walk();
  });
};

exports.revwalk = RevWalk;
