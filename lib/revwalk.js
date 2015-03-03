var NodeGit = require("../");
var Revwalk = NodeGit.Revwalk;
var Promise = require("nodegit-promise");

var oldSorting = Revwalk.prototype.sorting;

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
  var revwalk = this;

  this.push(oid);

  function walk() {
    revwalk.next().then(function(oid) {
      if (!oid) {
        if (typeof callback === "function") {
          return callback();
        }

        return;
      }

      revwalk.repo.getCommit(oid).then(function(commit) {
        if (typeof callback === "function") {
          callback(null, commit);
        }

        walk();
      });
    }, callback);
  }

  walk();
};


/**
 * Walk the history grabbing commits until the checkFn called with the
 * current commit returns false.
 *
 * @param  {Function} checkFn function returns false to stop walking
 * @return {Array}
 */
Revwalk.prototype.getCommitsUntil = function(checkFn) {
  var commits = [];
  var walker = this;

  function walkCommitsCb() {
    return walker.next().then(function(oid) {
      if (!oid) { return; }

      return walker.repo.getCommit(oid).then(function(commit) {
        commits.push(commit);
        if (checkFn(commit)) {
          return walkCommitsCb();
        }
      });
    });
  }

  return walkCommitsCb().then(function() {
    return commits;
  });
};

/**
 * Get a number of commits.
 *
 * @param  {Number} count (default: 10)
 * @return {Array<Commit>}
 */
Revwalk.prototype.getCommits = function(count) {
  count = count || 10;
  var promises = [];
  var walker = this;

  function walkCommitsCount(count) {
    if (count === 0) { return; }

    return walker.next().then(function(oid) {
      if (!oid) { return; }

      promises.push(walker.repo.getCommit(oid));
      return walkCommitsCount(count - 1);
    });
  }

  return walkCommitsCount(count).then(function() {
    return Promise.all(promises);
  });
};
