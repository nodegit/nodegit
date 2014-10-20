var NodeGit = require("../");
var normalizeOid = require("./util/normalize_oid");
var Blob = require("./blob");
var Tree = require("./tree");
var Reference = require("./refs");
var Revwalk = require("./revwalk");
var Commit = require("./commit");
var Remote = require("./remote");

var TreeBuilder = NodeGit.Treebuilder;
var Repository = NodeGit.Repository;

Object.defineProperty(Repository.prototype, "openIndex", {
  enumerable: false,
  value: Repository.prototype.index
});

/**
 * Look up a branch"s most recent commit.
 *
 * @param {String} name Branch name, e.g. "master"
 * @param {Function} callback
 * @return {Branch}
 */
Repository.prototype.getBranch = function(name, callback) {
  var repository = this;

  return this.getReference("refs/heads/" + name).then(function(reference) {
    return repository.getCommit(reference.target()).then(function(commit) {
      if (typeof callback === "function") {
        callback(null, commit);
      }

      return commit;
    });
  }, callback);
};

/**
 * Lists out the remotes in the given repository.
 *
 * @param {Function} Optional callback
 * @return {Object} Promise object.
 */
Repository.prototype.getRemotes = function(callback) {
  return Remote.list(this).then(function(remotes) {
    if (typeof callback === "function") {
      callback(null, remotes);
    }

    return remotes;
  }, callback);
};

/**
 * Lookup the reference with the given name.
 *
 * @param {String} name
 * @param {Function} callback
 * @return {Reference}
 */
Repository.prototype.getReference = function(name, callback) {
  var repository = this;

  return Reference.lookup(this, name).then(function(reference) {
    if (reference.type() == Reference.Type.Symbolic) {
      return reference.resolve(function (error, reference) {
        reference.repo = repository;

        if (callback) {
          callback(null, reference);
        }

        return reference;
      });
    } else {
      reference.repo = repository;
      if (callback) {
        callback(null, reference);
      }
      return reference;
    }
  }, callback);
};

/**
 * Retrieve the commit identified by oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Function} callback
 * @return {Commit}
 */
Repository.prototype.getCommit = function(oid, callback) {
  oid = normalizeOid(oid);

  var repository = this;

  return Commit.lookup(repository, oid).then(function(commit) {
    commit.repo = repository;

    if (typeof callback === "function") {
      callback(null, commit);
    }

    return commit;
  }, callback);
};

/**
 * Retrieve the blob represented by the oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Function} callback
 * @return {Blob}
 */
Repository.prototype.getBlob = function(oid, callback) {
  oid = normalizeOid(oid);

  var repository = this;

  return Blob.lookup(repository, oid).then(function(blob) {
    blob.repo = repository;

    if (typeof callback === "function") {
      callback(null, blob);
    }

    return blob;
  }, callback);
};

/**
 * Retrieve the tree represented by the oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Function} callback
 * @return {Tree}
 */
Repository.prototype.getTree = function(oid, callback) {
  oid = normalizeOid(oid);

  var repository = this;

  return Tree.lookup(repository, oid).then(function(tree) {
    tree.repo = repository;

    if (typeof callback === "function") {
      callback(null, tree);
    }

    return tree;
  }, callback);
};

/**
 * Retrieve the tag represented by the oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Function} callback
 * @return {Tag}
 */
Repository.prototype.getTag = function(oid, callback) {
  oid = normalizeOid(oid);

  var repository = this;

  return Reference.lookup(repository, oid).then(function(reference) {
    reference.repo = repository;

    if (typeof callback === "function") {
      callback(null, reference);
    }

    return reference;
  }, callback);
};

/**
 * Instantiate a new revision walker for browsing the Repository"s history.
 * See also `Commit.prototype.history()`
 *
 * @param {String|Oid} String sha or Oid
 * @param {Function} callback
 * @return {RevWalk}
 */
Repository.prototype.createRevWalk = function() {
  var revWalk = Revwalk.createRevwalk(this);
  revWalk.repo = this;
  return revWalk;
};

/**
 * Retrieve the master branch.
 *
 * @param {Function} callback
 * @return {Branch}
 */
Repository.prototype.getMaster = function(callback) {
  return this.getBranch("master", callback);
};

/**
 * Create a commit
 *
 * @param {String} updateRef
 * @param {Signature} author
 * @param {Signature} committer
 * @param {String} message
 * @param {Tree|Oid|String} Tree
 * @param {Array} parents
 * @param {Function} callback
 * @return {Oid} The oid of the commit
 */
Repository.prototype.createCommit = function(
  updateRef, author, committer, message, tree, parents, callback) {

  var createCommit = null;
  var commit = this;

  if (tree instanceof Tree) {
    commit = Commit.create.call(
      this,
      updateRef,
      author,
      committer,
      null /* use default message encoding */,
      message,
      tree,
      parents.length,
      parents
    );
  } else {
    createCommit = this.getTree(tree).then(function(tree) {
      return Commit.create.call(
        commit,
        updateRef,
        author,
        committer,
        null /* use default message encoding */,
        message,
        tree,
        parents.length,
        parents
      );
    });
  }

  return createCommit.then(function(commit) {
    if (typeof callback === "function") {
      callback(null, commit);
    }

    return commit;
  }, callback);
};

/**
 * Create a blob from a buffer
 *
 * @param {Buffer} buffer
 * @param {Function} callback
 * @return {Blob}
 */
Repository.prototype.createBlobFromBuffer = function(buffer, callback) {
  return Blob.createFrombuffer.call(this, buffer, buffer.length, callback);
};

/**
 * Create a new tree builder.
 *
 * @param {Tree} tree
 */
Repository.prototype.treeBuilder = function() {
  var builder = TreeBuilder.create(null);

  builder.root = builder;
  builder.repo = this;

  return builder;
};

module.exports = Repository;
