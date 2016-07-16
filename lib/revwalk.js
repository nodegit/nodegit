var NodeGit = require("../");
var Revwalk = NodeGit.Revwalk;

Object.defineProperty(Revwalk.prototype, "repo", {
  get: function () { return this.repository(); }
});

/**
 * @typedef historyEntry
 * @type {Object}
 * @property {Commit} commit the commit for this entry
 * @property {Number} status the status of the file in the commit
 * @property {String} newName the new name that is provided when status is
 *                            renamed
 * @property {String} oldName the old name that is provided when status is
 *                            renamed
 */
var fileHistoryWalk = Revwalk.prototype.fileHistoryWalk;
/**
 * @param {String} filePath
 * @param {Number} max_count
 * @async
 * @return {Array<historyEntry>}
 */
Revwalk.prototype.fileHistoryWalk = fileHistoryWalk;

/**
 * Get a number of commits.
 *
 * @async
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
      promises.push(walker.repo.getCommit(oid));
      return walkCommitsCount(count - 1);
    })
    .catch(function(error) {
      if (error.errno !== NodeGit.Error.CODE.ITEROVER) {
        throw error;
      }
    });
  }

  return walkCommitsCount(count).then(function() {
    return Promise.all(promises);
  });
};

/**
 * Walk the history grabbing commits until the checkFn called with the
 * current commit returns false.
 *
 * @async
 * @param  {Function} checkFn function returns false to stop walking
 * @return {Array}
 */
Revwalk.prototype.getCommitsUntil = function(checkFn) {
  var commits = [];
  var walker = this;

  function walkCommitsCb() {
    return walker.next().then(function(oid) {
      return walker.repo.getCommit(oid).then(function(commit) {
        commits.push(commit);
        if (checkFn(commit)) {
          return walkCommitsCb();
        }
      });
    })
    .catch(function(error) {
      if (error.errno !== NodeGit.Error.CODE.ITEROVER) {
        throw error;
      }
    });
  }

  return walkCommitsCb().then(function() {
    return commits;
  });
};

/**
 * Set the sort order for the revwalk. This function takes variable arguments
 * like `revwalk.sorting(NodeGit.RevWalk.Topological, NodeGit.RevWalk.Reverse).`
 *
 * @param {Number} sort
 */
var oldSorting = Revwalk.prototype.sorting;
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
    revwalk.next().done(function(oid) {
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
