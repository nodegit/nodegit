var git = require("../");
var util = require("./util");
var Revwalk = git.Revwalk;

// Backwards compatibility.
Object.defineProperty(git, "RevWalk", {
  value: Revwalk,
  enumerable: false
});

var oldSorting = Revwalk.prototype.sorting;

/**
 * Refer to vendor/libgit2/include/git2/revwalk.h for sort definitions.
 */
Revwalk.Sort = {
  None: 0,
  Topological: 1,
  Time: 2,
  Reverse: 4
};

/**
 * Set the sort order for the revwalk. This function takes variable arguments
 * like `revwalk.sorting(git.RevWalk.Topological, git.RevWalk.Reverse).`
 *
 * @param {Number} sort
 */
Revwalk.prototype.sorting = function() {
  var sort = 0;

  for (var i = 0; i < arguments.length; i++) {
    sort |= arguments[i];
  }

  oldSorting.call(this, sort);
};

/**
 * Walk the history from the given oid. The callback is invoked for each commit;
 * When the walk is over, the callback is invoked with `(null, null)`.
 *
 * @param  {Oid} oid
 * @param  {Function} callback
 * @return {Commit}
 */
Revwalk.prototype.walk = function(oid, callback) {
  var self = this;

  this.push(oid);

  function walk() {
    self.next(function revWalkNext(error, oid) {
      if (error) {
        return callback(error);
      }

      if (!oid) {
        return callback();
      }

      self.repo.getCommit(oid, function revWalkCommitLookup(error, commit) {
        if (error) {
          return callback(error);
        }

        callback(null, commit);
        walk();
      });
    });
  }

  walk();
};

util.normalizeOid(Revwalk.prototype, "walk");
util.makeSafe(Revwalk.prototype, "walk");
