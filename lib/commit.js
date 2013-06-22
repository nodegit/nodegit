var git = require( '../' ),
  success = require('./utilities').success,
  events = require('events');

/**
 * Convenience commit constructor.
 *
 * @constructor
 * @param {git.raw.Repo} rawRepo Raw repository object.
 * @param  {git.raw.Commit} [rawCommit = new git.raw.Commit(rawRepo)] Raw commit object.
 */
var Commit = function(repo, rawCommit) {
  if (!(repo instanceof git.repo)) {
    throw new git.error('First parameter for Commit must be a raw repo');
  }
  this.repo = repo;

  if (!(rawCommit instanceof git.raw.Commit)) {
    throw new git.error('Second parameter for Commit must be a raw commit');
  }
  this.rawCommit = rawCommit;
};

/**
 * Retrieve the commit's OID.
 */
Commit.prototype.oid = function() {
  return new git.oid(this.rawCommit.oid());
};

/**
 * Retrieve the SHA.
 */
Commit.prototype.sha = function() {
  return this.oid().sha();
};

/**
 * Retrieve the message
 */
Commit.prototype.message = function() {
  return this.rawCommit.message();
};

/**
 * Retrieve the commit time as a unix timestamp.
 */
Commit.prototype.time = function() {
  return this.rawCommit.time() * 1000;
};

/**
 * Retrieve the commit time as a Date object.
 */
Commit.prototype.date = function() {
  return new Date(this.time());
};

/**
 * Retrieve the commit's positive or negative timezone offset, in minutes from UTC.
 */
Commit.prototype.offset = function(callback) {
  return this.rawCommit.offset();
};

/**
 * Retrieve the commit's author signature.
 */
Commit.prototype.author = function() {
  return this.rawCommit.author();
};

/**
 * Retrieve the commit's committer.
 */
Commit.prototype.committer = function() {
  return this.rawCommit.committer();
};

/**
 * Retrieve the tree for this commit.
 *
 * @param {Commit~getTreeCallback} callback
 */
Commit.prototype.getTree = function(callback) {
  /**
   * @callback Commit~getTreeCallback Callback executed on tree retrieval
   * @param {GitError|null} error An Error or null if successful.
   * @param {Tree|null} Tree
   */

  var self = this;
  this.rawCommit.tree(function(error, rawTree) {
    if (error) return callback(error);

    callback(null, new git.tree(self.repo, rawTree));
  });
};

/**
 * Retrieve the file represented by path for this commit.
 * Path must be relative to repository root.
 *
 * @param {String} path
 * @param {Commit~fileCallback} callback
 */
Commit.prototype.file = function(path, callback) {
  /**
   * @callback Commit~fileCallback Callback executed on file retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Entry|null} file Retrieved file entry.
   */
  this.getTree(function (error, tree) {
    if (error) return callback(error);
    tree.entry(path, function(error, entry) {
      if (success(error, callback)) {
        callback(null, entry);
      }
    });
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
  var event = new events.EventEmitter(),
      self = this;

  var oid = self.oid();
  (new git.revwalk(self.repo)).allocate(function createRevwalk(error, revwalk) {
    var commits = [];
    revwalk.walk(oid, function commitRevWalk(error, index, commit, noMoreCommits) {
      if(error) {
        event.emit('end', error, commits);
        return false;
      }

      if (noMoreCommits) {
        /**
         * End event.
         *
         * @event Commit#end
         *
         * @param {GitError|null} error An error object if there was an issue, null otherwise.
         * @param {Commit[]} commits The commits.
         */
        event.emit('end', null, commits);
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
      event.emit('commit', null, commit);
      commits.push(commit);
    });
  });
  return event;
};

/**
 * Retrieve the commit's parents.
 *
 * @param {Commit~parentsCallback} callback
 */
Commit.prototype.parents = function(callback) {
  /**
   * @callback Commit~parentsCallback Callback executed on parents retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Commit[]|null} parents Commit's parent(s).
   */
  var self = this;

  function processParents(rawCommit, n, acc, callback) {
    if (n < 0) return callback(null, acc);

    rawCommit.parent(n, function nextParent(error, rawParentCommit) {
      if (success(error, callback)) {
        processParents(rawParentCommit, n-1, acc.concat([new Commit(self.repo, rawParentCommit)]), callback)
      }
    });
  }

  processParents(this.rawCommit, this.rawCommit.parentCount() - 1, [], callback);
};

/**
 * Generate an array of diff trees showing changes between this commit 
 * and its parent(s).
 *
 * @param {Commit~parentsDiffTreesCallback} callback
 */
Commit.prototype.parentsDiffTrees = function(callback) {
  /**
   * @callback Commit~parentsDiffTreesCallback Callback executed on diff trees retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {DiffList[]|null} diffLists Array of DiffTrees showing changes between this commit and its parent(s)
   */
  var self = this;
  var commitSha = self.sha();
  self.parents(function commitParents(error, parents) {
    if (!success(error, callback)) return;

    var parentDiffLists = [];
    parents.forEach(function commitEachParent(parent) {
      var parentSha = parent.sha();
      git.diffList.treeToTree(self.repo, parentSha, commitSha, function walkDiffList(error, diffList) {
        if (!success(error, callback))  return;

        parentDiffLists.push(diffList);
        if (parentDiffLists.length === parents.length) {
          callback(null, parentDiffLists);
        }
      });
    });
  });
};

exports.commit = Commit;
