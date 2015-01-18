var events = require("events");
var Promise = require("nodegit-promise");
var NodeGit = require("../");
var LookupWrapper = require("./util/lookupWrapper");

var Commit = NodeGit.Commit;

/**
 * Retrieves the commit pointed to by the oid
 * @param {Repository} repo The repo that the commit lives in
 * @param {String|Oid|Commit} id The commit to lookup
 * @param {Function} callback
 * @return {Commit}
 */
Commit.lookup = LookupWrapper(Commit);

/**
 * Retrieve the SHA.
 * @return {String}
 */
Commit.prototype.sha = function() {
  return this.id().toString();
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
 *
 * @param {Function} callback
 * @return {Tree}
 */
Commit.prototype.getTree = function(callback) {
  return this.repo.getTree(this.treeId(), callback);
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
  return this.getTree().then(function(tree) {
    return tree.getEntry(path).then(function(entry) {
      if (typeof callback === "function") {
        callback(null, entry);
      }

      return entry;
    });
  }, callback);
};

/**
 * Walk the history from this commit backwards.
 * An EventEmitter is returned that will emit a "commit" event for each
 * commit in the history, and one "end" event when the walk is completed.
 * Don"t forget to call `start()` on the returned event.
 *
 * @fires Commit#commit
 * @fires Commit#end
 *
 * @return {EventEmitter}
 */
Commit.prototype.history = function() {
  var event = new events.EventEmitter();
  var oid = this.id();
  var revwalk = this.repo.createRevWalk();

  revwalk.sorting.apply(revwalk, arguments);

  var commits = [];

  event.start = function() {
    revwalk.walk(oid, function commitRevWalk(error, commit) {
      if (error) {
        return event.emit("error", error);
      }

      if (!commit) {
        event.emit("end", commits);
        return;
      }

      event.emit("commit", commit);
      commits.push(commit);
    });
  };

  return event;
};

/**
 * Retrieve the commit"s parents -- as commit objects.
 *
 * @param {number} limit - Optional amount of parents to return.
 * @param {Function} callback
 * @return {[Commit]} array of commits
 */
Commit.prototype.getParents = function(limit, callback) {
  var parents = [];

  // Shift arguments.
  if (typeof limit === "function") {
    callback = limit;
  }

  // If no limit was set, default to the maximum parents.
  limit = typeof limit === "number" ? limit : this.parentcount();
  limit = Math.min(limit, this.parentcount());

  for (var i = 0; i < limit; i++) {
    var oid = this.parentId(i);
    var parent = this.repo.getCommit(oid);

    parents.push(parent);
  }

  // Wait for all parents to complete, before returning.
  return Promise.all(parents).then(function(parents) {
    if (typeof callback === "function") {
      callback(null, parents);
    }

    return parents;
  }, callback);
};

/**
 * Retrieve the commit"s parent shas.
 *
 * @param {Function} callback
 * @return {[Oid]} array of oids
 */
Commit.prototype.parents = function() {
  var result = [];

  for (var i = 0; i < this.parentcount(); i++) {
    result.push(this.parentId(i));
  }

  return result;
};

/**
 * Generate an array of diff trees showing changes between this commit
 * and its parent(s).
 *
 * @param {Function} callback
 * @return {[Diff]} an array of diffs
 */
Commit.prototype.getDiff = function(callback) {
  var commit = this;

  return commit.getParents().then(function(parents) {
    var diffs = parents.map(function(parent) {
      return parent.getTree().then(function(parentTree) {
        return commit.getTree().then(function(thisTree) {
          return parentTree.diff(thisTree);
        });
      });
    });

    return Promise.all(diffs);
  }).then(function(diffs) {
    if (typeof callback === "function") {
      callback(null, diffs);
    }

    return diffs;
  }, callback);
};

/**
 * The sha of this commit
 * @return {String}
 */
Commit.prototype.toString = function() {
  return this.sha();
};

module.exports = Commit;
