var events = require("events");
var fp = require("lodash/fp");
var NodeGit = require("../");
var Commit = NodeGit.Commit;
var LookupWrapper = NodeGit.Utils.lookupWrapper;

var _amend = Commit.prototype.amend;
var _parent = Commit.prototype.parent;

/**
 * Retrieves the commit pointed to by the oid
 * @async
 * @param {Repository} repo The repo that the commit lives in
 * @param {String|Oid|Commit} id The commit to lookup
 * @return {Commit}
 */
Commit.lookup = LookupWrapper(Commit);

/**
 * @async
 * @param {Number} n
 * @return {Commit}
 */
Commit.prototype.parent = function(n) {
  var repo = this.repo;
  return _parent.call(this, n).then(p => {
    p.repo = repo;
    return p;
  });
};

/**
 * Amend a commit
 * @async
 * @param {String} update_ref
 * @param {Signature} author
 * @param {Signature} committer
 * @param {String} message_encoding
 * @param {String} message
 * @param {Tree|Oid} tree
 * @return {Oid}
 */
Commit.prototype.amend = function (
  updateRef, author, committer, message_encoding, message, tree) {
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
 * Amend a commit with the given signature
 * @async
 * @param {String} updateRef
 * @param {Signature} author
 * @param {Signature} committer
 * @param {String} messageEncoding
 * @param {String} message
 * @param {Tree|Oid} tree
 * @param {Function} onSignature Callback to be called with string to be signed
 * @return {Oid}
*/
Commit.prototype.amendWithSignature = function(
  updateRef,
  author,
  committer,
  messageEncoding,
  message,
  tree,
  onSignature
) {
    let repo = this.repo;
    let parentOids = this.parents();
    let _this = this;
    let promises = [];

    if (tree instanceof NodeGit.Oid) {
      promises.push(repo.getTree(tree));
    } else {
      promises.push(Promise.resolve(tree));
    }

    parentOids.forEach(function (parentOid) {
      promises.push(repo.getCommit(parentOid));
    });

    let treeObject;
    let parents;
    let commitContent;
    let commit;
    let skippedSigning;
    let resolvedAuthor;
    let resolvedCommitter;
    let resolvedMessageEncoding;
    let resolvedMessage;
    let resolvedTree;

    let createCommitPromise = Promise.all(promises)
      .then(function(results) {
        treeObject = fp.head(results);
        parents = fp.tail(results);
        return _this.getTree();
      })
      .then(function(commitTreeResult) {
        let commitTree = commitTreeResult;

        let truthyArgs = fp.omitBy(
          fp.isNil,
          {
            author,
            committer,
            messageEncoding,
            message,
            tree: treeObject
          }
        );

        let commitFields = {
          author: _this.author(),
          committer: _this.committer(),
          messageEncoding: _this.messageEncoding(),
          message: _this.message(),
          tree: commitTree
        };

        ({
          author: resolvedAuthor,
          committer: resolvedCommitter,
          messageEncoding: resolvedMessageEncoding,
          message: resolvedMessage,
          tree: resolvedTree
        } = fp.assign(
          commitFields,
          truthyArgs
        ));

        return Commit.createBuffer(
          repo,
          resolvedAuthor,
          resolvedCommitter,
          resolvedMessageEncoding,
          resolvedMessage,
          resolvedTree,
          parents.length,
          parents
        );
      })
      .then(function(commitContentResult) {
        commitContent = commitContentResult;
        if (!commitContent.endsWith("\n")) {
          commitContent += "\n";
        }
        return onSignature(commitContent);
      })
      .then(function({ code, field, signedData }) {
        switch (code) {
          case NodeGit.Error.CODE.OK:
            return Commit.createWithSignature(
              repo,
              commitContent,
              signedData,
              field
            );
          case NodeGit.Error.CODE.PASSTHROUGH:
            skippedSigning = true;
            return Commit.create(
              repo,
              updateRef,
              resolvedAuthor,
              resolvedCommitter,
              resolvedMessageEncoding,
              resolvedMessage,
              resolvedTree,
              parents.length,
              parents
            );
          default: {
            const error = new Error(
              `Commit.amendWithSignature threw with error code ${code}`
            );
            error.errno = code;
            throw error;
          }
        }
      });

    if (!updateRef) {
      return createCommitPromise;
    }

    return createCommitPromise
      .then(function(commitOid) {
        if (skippedSigning) {
          return commitOid;
        }

        return repo.getCommit(commitOid)
          .then(function(commitResult) {
            commit = commitResult;
            return repo.getReference(updateRef);
          }).then(function(ref) {
            return ref.setTarget(
              commitOid,
              `commit (amend): ${commit.summary()}`
            );
          }).then(function() {
            return commitOid;
          });
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
 * @return {Array<Diff>} an array of diffs
 */
Commit.prototype.getDiff = function() {
  return this.getDiffWithOptions(null);
};

/**
 * Generate an array of diff trees showing changes between this commit
 * and its parent(s).
 *
 * @async
 * @param {Object} options
 * @return {Array<Diff>} an array of diffs
 */
Commit.prototype.getDiffWithOptions = function(options) {
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
  });
};

/**
 * Retrieve the entry represented by path for this commit.
 * Path must be relative to repository root.
 *
 * @async
 * @param {String} path
 * @return {TreeEntry}
 */
Commit.prototype.getEntry = function(path) {
  return this.getTree().then(function(tree) {
    return tree.getEntry(path);
  });
};

/**
 * Retrieve the commit's parents as commit objects.
 *
 * @async
 * @param {number} limit Optional amount of parents to return.
 * @return {Array<Commit>} array of commits
 */
Commit.prototype.getParents = function(limit) {
  var parents = [];

  // If no limit was set, default to the maximum parents.
  limit = typeof limit === "number" ? limit : this.parentcount();
  limit = Math.min(limit, this.parentcount());

  for (var i = 0; i < limit; i++) {
    var oid = this.parentId(i);
    var parent = this.repo.getCommit(oid);

    parents.push(parent);
  }

  // Wait for all parents to complete, before returning.
  return Promise.all(parents);
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
Commit.prototype.getTree = function() {
  return this.repo.getTree(this.treeId());
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
 * Get the specified parent of the commit.
 *
 * @param {number} the position of the parent, starting from 0
 * @async
 * @return {Commit} the parent commit at the specified position
 */
Commit.prototype.parent = function (id) {
  var repository = this.repo;
  return _parent.call(this, id).then(function(parent) {
    parent.repo = repository;
    return parent;
  });
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
