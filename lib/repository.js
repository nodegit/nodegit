var NodeGit = require("../");
var normalizeOid = require("./util/normalize_oid");
var Blob = require("./blob");
var Tree = require("./tree");
var Reference = require("./reference");
var Revwalk = require("./revwalk");
var Commit = require("./commit");

var TreeBuilder = NodeGit.Treebuilder;
var Repository = NodeGit.Repository;

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
      if (callback) {
        callback(null, commit);
      }

      return commit;
    });
  });
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
        if (error) {
          return callback(error);
        }

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
  });
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

    if (callback) {
      callback(null, commit);
    }

    return commit;
  });
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

    if (callback) {
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

    if (callback) {
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

    if (callback) {
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
  this.getBranch("master", callback);
};

/**
 * Create a commit
 *
 * @param {String} updateRef
 * @param {Signature} author
 * @param {Signature} commiter
 * @param {String} message
 * @param {Tree|Oid|String} Tree
 * @param {Array} parents
 * @param {Function} callback
 * @return {Oid} The oid of the commit
 */
Repository.prototype.createCommit = function(
  // FIXME
  updateRef, author, committer, message, tree, parents, callback) {
  if (tree instanceof Tree) {
    Commit.createCommit.call(
      this,
      updateRef,
      author,
      committer,
      null /* use default message encoding */,
      message,
      tree,
      parents.length, parents,
      callback);
  } else {
    var self = this;
    this.getTree(tree, function(error, tree) {
      if (error) {
        return callback(error);
      }

      Commit.createCommit.call(
        self,
        updateRef,
        author,
        committer,
        null /* use default message encoding */,
        message,
        tree,
        parents.length, parents,
        callback);
    });
  }
};

/**
 * Create a blob from a buffer
 *
 * @param {Buffer} buffer
 * @param {Function} callback
 * @return {Blob}
 */
Repository.prototype.createBlobFromBuffer = function(buffer, callback) {
  Blob.createFrombuffer.call(this, buffer, buffer.length, callback);
};

/**
 * Create a new tree builder.
 *
 * @param {Tree} tree
 * @param {Function} callback
 */
Repository.prototype.treeBuilder = function(callback) {
  var builder = TreeBuilder.create(null);
  builder.root = builder;
  builder.repo = this;
  return builder;
};

module.exports = Repository;
