var git = require('../'),
    success = require('./utilities').success;

/**
 * Convenience revision walking class
 *
 * @constructor
 * @param {git.raw.Repo} rawRepo
 * @param {git.raw.RevWalk|null} rawRevWalk
 */
var RevWalk = function(rawRepo, rawRevWalk) {
  if (!(rawRepo instanceof git.raw.Repo)) {
    throw git.error('First parameter for RevWalk must be a raw repo');
  }
  this.rawRepo = rawRepo;

  if (rawRevWalk instanceof git.raw.RevWalk) {
    this.rawRevWalk = rawRevWalk;
  } else {
    this.rawRevWalk = new git.raw.RevWalk(this.rawRepo);
  }
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
  self.rawRevWalk.push(oid.getRawOid(), function revWalkPush(error) {
    if (!success(error, callback)) {
      return;
    }

    var shouldContinue = true, index = 0;

    function walk() {
      if(!shouldContinue) {
        return;
      }

      self.rawRevWalk.next(function revWalkNext(error, oid, walkOver) {
        if(error) {
          callback(git.error(error), index, commit);
          return;
        }

        // Finished walking history, apply callback with noMoreCommits = true
        if (walkOver) {
          callback(null, index, null, walkOver);
          return;
        }

        (new git.commit(self.rawRepo)).lookup(oid, function revWalkCommitLookup(error, commit) {
          if(error) {
            callback(git.error(error), index, commit);
            return;
          }
          if(callback(null, index, commit) === false) {
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

exports.revwalk = RevWalk;
