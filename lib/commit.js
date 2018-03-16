var events = require("events");
var NodeGit = require("../");
var Commit = NodeGit.Commit;
var LookupWrapper = NodeGit.Utils.lookupWrapper;

var _amend = Commit.prototype.amend;

/**
 * Retrieves the commit pointed to by the oid
 * @async
 * @param {Repository} repo The repo that the commit lives in
 * @param {String|Oid|Commit} id The commit to lookup
 * @return {Commit}
 */
Commit.lookup = LookupWrapper(Commit);

/**
 * Amend a commit
 * @async
 * @param {String} update_ref
 * @param {Signature} author
 * @param {Signature} committer
 * @param {String} message_encoding
 * @param {String} message
 * @param {Tree|Oid} tree
 * @param {Oid} callback
 */
Commit.prototype.amend = function (
  updateRef, author, committer, message_encoding, message, tree, callback) {
    var repo = this.repo;
    var _this = this;
    var treePromise;

    if (tree instanceof NodeGit.Oid){
      treePromise = repo.getTree(tree);
    } else {
      treePromise = Promise.resolve(tree);
    }

    return treePromise
      .then(function(treeObject){
        return _amend.call(_this,
          updateRef,
          author,
          committer,
          message_encoding,
          message,
          treeObject
        );
      });
};

/**
 * Retrieve the commit time as a Date object.
 * @return {Date}
 */
Commit.prototype.date = function() {
  return new Date(this.timeMs());
};

/**
 * Generate an array of diff trees showing changes between this commit
 * and its parent(s).
 *
 * @async
 * @param {Function} callback
 * @return {Array<Diff>} an array of diffs
 */
Commit.prototype.getDiff = function(callback) {
  return this.getDiffWithOptions(null, callback);
};

/**
 * Generate an array of diff trees showing changes between this commit
 * and its parent(s).
 *
 * @async
 * @param {Object} options
 * @param {Function} callback
 * @return {Array<Diff>} an array of diffs
 */
Commit.prototype.getDiffWithOptions = function(options, callback) {
  var commit = this;

  return commit.getTree().then(function(thisTree) {
    return commit.getParents().then(function(parents) {
      var diffs;
      if (parents.length) {
        diffs = parents.map(function(parent) {
          return parent.getTree().then(function(parentTree) {
            return thisTree.diffWithOptions(parentTree, options);
          });
        });
      } else {
        diffs = [thisTree.diffWithOptions(null, options)];
      }

      return Promise.all(diffs);
    });
  }).then(function(diffs) {
    if (typeof callback === "function") {
      callback(null, diffs);
    }

    return diffs;
  }, callback);
};

/**
 * Retrieve the entry represented by path for this commit.
 * Path must be relative to repository root.
 *
 * @async
 * @param {String} path
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
 * Retrieve the commit's parents as commit objects.
 *
 * @async
 * @param {number} limit Optional amount of parents to return.
 * @param {Function} callback
 * @return {Array<Commit>} array of commits
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
 * @typedef extractedSignature
 * @type {Object}
 * @property {String} signature the signature of the commit
 * @property {String} signedData the extracted signed data
 */

/**
 * Retrieve the signature and signed data for a commit.
 * @param  {String} field Optional field to get from the signature,
 *                        defaults to gpgsig
 * @return {extractedSignature}
 */
Commit.prototype.getSignature = function(field) {
  return Commit.extractSignature(this.repo, this.id(), field);
};

/**
 * Get the tree associated with this commit.
 *
 * @async
 * @return {Tree}
 */
Commit.prototype.getTree = function(callback) {
  return this.repo.getTree(this.treeId(), callback);
};

/**
 * Walk the history from this commit backwards.
 *
 * An EventEmitter is returned that will emit a "commit" event for each
 * commit in the history, and one "end" event when the walk is completed.
 * Don't forget to call `start()` on the returned event.
 *
 * @fires EventEmitter#commit Commit
 * @fires EventEmitter#end Array<Commit>
 * @fires EventEmitter#error Error
 *
 * @return {EventEmitter}
 * @start start()
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
        if (error.errno === NodeGit.Error.CODE.ITEROVER) {
          event.emit("end", commits);
          return;
        } else {
          return event.emit("error", error);
        }
      }

      event.emit("commit", commit);
      commits.push(commit);
    });
  };

  return event;
};

/**
 * Retrieve the commit's parent shas.
 *
 * @return {Array<Oid>} array of oids
 */
Commit.prototype.parents = function() {
  var result = [];

  for (var i = 0; i < this.parentcount(); i++) {
    result.push(this.parentId(i));
  }

  return result;
};

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
 * The sha of this commit
 * @return {String}
 */
Commit.prototype.toString = function() {
  return this.sha();
};
