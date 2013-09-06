var git = require('../'),
    Commit = git.Commit,
    events = require('events');

/**
 * Retrieve the SHA.
 * @return {String}
 */
Commit.prototype.sha = function() {
  return this.oid().sha();
};

/**
 * Retrieve the commit time as a unix timestamp.
 * @return {Number}
 */
Commit.prototype.timeMs = function() {
  return this.time() * 1000;
};

/**
 * Retrieve the commit time as a Date object.
 * @return {Date}
 */
Commit.prototype.date = function() {
  return new Date(this.timeMs());
};

/**
 * Get the tree associated with this commit.
 * @return {Tree}
 */
Commit.prototype.getTree = function(callback) {
  this.repo.getTree(this.treeId(), callback);
};

/**
 * Retrieve the entry represented by path for this commit.
 * Path must be relative to repository root.
 *
 * @param {String} path
 * @param {Function} callback
 * @return {TreeEntry}
 */
Commit.prototype.getEntry = function(path, callback) {
  this.getTree(function(error, tree) {
    if (error) return callback(error);

    tree.getEntry(path, callback);
  });
};

/**
 * Walk the history from this commit backwards.
 * An EventEmitter is returned that will emit a 'commit' event for each 
 * commit in the history, and one 'end' event when the walk is completed.
 * Don't forget to call `start()` on the returned event.
 *
 * @fires Commit#commit
 * @fires Commit#end
 *
 * @return {EventEmitter}
 */
Commit.prototype.history = function() {
  var event = new events.EventEmitter();

  var oid = this.oid();
  var revwalk = this.repo.createRevWalk();
  revwalk.sorting.apply(revwalk, arguments);

  var commits = [];
  event.start = function() {
    revwalk.walk(oid, function commitRevWalk(error, commit) {
      if (error) return event.emit('error', error);

      if (!commit) {
        event.emit('end', commits);
        return;
      }
      event.emit('commit', commit);
      commits.push(commit);
    });
  };
  return event;
};

/**
 * Retrieve the commit's parents -- as commit objects.
 *
 * @param {Function} callback
 * @return {[Commit]} array of commits
 */
Commit.prototype.getParents = function(callback) {
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
 * Retrieve the commit's parent shas.
 *
 * @param {Function} callback
 * @return {[Oid]} array of oids
 */
Commit.prototype.parents = function() {
  var result = [];
  for (var i = 0; i < this.parentCount(); i++) {
    result.push(this.parentId(i));
  }
  return result;
}

/**
 * Generate an array of diff trees showing changes between this commit 
 * and its parent(s).
 *
 * @param {Function} callback
 * @return {[DiffList]} an array of difflists
 */
Commit.prototype.getDiff = function(callback) {
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

/**
 * The sha of this commit
 * @return {String}
 */
Commit.prototype.toString = function() {
  return this.sha();
};
