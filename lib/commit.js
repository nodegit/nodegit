var git = require('../'),
    Commit = git.Commit,
    events = require('events');

/**
 * Retrieve the SHA.
 */
Commit.prototype.sha = function() {
  return this.oid().sha();
};

/**
 * Retrieve the commit time as a unix timestamp.
 */
Commit.prototype.timeMs = function() {
  return this.time() * 1000;
};

/**
 * Retrieve the commit time as a Date object.
 */
Commit.prototype.date = function() {
  return new Date(this.timeMs());
};

/**
 * Get the tree associated with this commit.
 */
Commit.prototype.getTree = function(callback) {
  this.repo.getTree(this.treeId(), callback);
};

/**
 * Retrieve the entry represented by path for this commit.
 * Path must be relative to repository root.
 *
 * @param {String} path
 * @param {Commit~fileCallback} callback
 */
Commit.prototype.getEntry = function(path, callback) {
  /**
   * @callback Commit~fileCallback Callback executed on file retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Entry|null} file Retrieved file entry.
   */
  this.getTree(function(error, tree) {
    if (error) return callback(error);

    tree.getEntry(path, callback);
  });
};

/**
 * Walk the history from this commit backwards.
 * An EventEmitter is returned that will emit a 'commit' event for each 
 * commit in the history, and one 'end' event when the walk is completed.
 *
 * @fires Commit#commit
 * @fires Commit#end
 *
 * @return {EventEmitter} historyWalkEmitter
 */
Commit.prototype.history = function() {
  var event = new events.EventEmitter();

  var oid = this.oid();
  var revwalk = this.repo.createRevWalk();
  var commits = [];
  event.start = function() {
    revwalk.walk(oid, function commitRevWalk(error, commit) {
      if (error) return event.emit('error', error);

      if (!commit) {
        /**
         * End event.
         *
         * @event Commit#end
         *
         * @param {GitError|null} error An error object if there was an issue, null otherwise.
         * @param {Commit[]} commits The commits.
         */
        event.emit('end', commits);
        return;
      }
      /**
       * Commit event.
       *
       * @event Commit#commit
       *
       * @param {GitError|null} error An error object if there was an issue, null otherwise.
       * @param {Commit} commit The commit.
       */
      event.emit('commit', commit);
      commits.push(commit);
    });
  };
  return event;
};

/**
 * Retrieve the commit's parents.
 *
 * @param {Commit~parentsCallback} callback
 */
Commit.prototype.getParents = function(callback) {
  /**
   * @callback Commit~parentsCallback Callback executed on parents retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Commit[]|null} parents Commit's parent(s).
   */
  var self = this;
  function processParents(commit, n, acc, callback) {
    if (n < 0) return callback(null, acc);

    self.repo.getCommit(self.parentId(n), function nextParent(error, parent) {
      if (error) return callback(error);
      processParents(parent, n-1, acc.concat([parent]), callback);
    });
  }

  processParents(this, this.parentCount() - 1, [], callback);
};

/**
 * Generate an array of diff trees showing changes between this commit 
 * and its parent(s).
 *
 * @param {Commit~parentsDiffTreesCallback} callback
 */
Commit.prototype.getDiff = function(callback) {
  /**
   * @callback Commit~parentsDiffTreesCallback Callback executed on diff trees retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {DiffList[]|null} diffLists Array of DiffTrees showing changes between this commit and its parent(s)
   */
  var self = this;
  self.getParents(function commitParents(error, parents) {
    if (error) return callback(error);

    var parentDiffLists = [];
    parents.forEach(function commitEachParent(parent) {
      parent.getTree(function(error, parentTree) {
        if (error) return callback(error);

        self.getTree(function(error, thisTree) {
          if (error) return callback(error);

          parentTree.diff(thisTree, function walkDiffList(error, diffList) {
            if (error) return callback(error);

            parentDiffLists.push(diffList);
            if (parentDiffLists.length === parents.length) {
              callback(null, parentDiffLists);
            }
          });
        });
      });
    });
  });
};
