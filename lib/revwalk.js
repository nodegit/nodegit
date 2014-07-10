var NodeGit = require("../");
var normalizeOid = require("./util/normalize_oid");

var Revwalk = NodeGit.Revwalk;

// Backwards compatibility.
Object.defineProperty(NodeGit, "RevWalk", {
  value: Revwalk,
  enumerable: false
});

var oldSorting = Revwalk.prototype.sorting;

/**
 * Refer to vendor/libNodeGit2/include/NodeGit2/revwalk.h for sort definitions.
 */
Revwalk.Sort = {
  None: 0,
  Topological: 1,
  Time: 2,
  Reverse: 4
};

/**
 * Set the sort order for the revwalk. This function takes variable arguments
 * like `revwalk.sorting(NodeGit.RevWalk.Topological, NodeGit.RevWalk.Reverse).`
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
  oid = normalizeOid(oid);

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

module.exports = Revwalk;
