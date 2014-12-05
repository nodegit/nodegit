var NodeGit = require("../");
var Blob = require("./blob");
var Tree = require("./tree");
var Tag = require("./tag");
var Reference = require("./reference");
var Revwalk = require("./revwalk");
var Commit = require("./commit");
var Remote = require("./remote");
var Promise = require("nodegit-promise");
var normalizeOptions = require("./util/normalize_options");

var TreeBuilder = NodeGit.Treebuilder;
var Repository = NodeGit.Repository;

Object.defineProperty(Repository.prototype, "openIndex", {
  enumerable: false,
  value: Repository.prototype.index
});

/**
 * Creates a branch with the passed in name pointing to the commit
 *
 * @param {String} name Branch name, e.g. "master"
 * @param {Commit|String|Oid} commit The commit the branch will point to
 * @param {bool} force Overwrite branch if it exists
 * @param {Signature} signature Identity to use to populate reflog
 * @param {String} logMessage One line message to be appended to the reflog
 * @return {Ref}
 */
Repository.prototype.createBranch =
function(name, commit, force, signature, logMessage) {
  var repo = this;

  if (commit instanceof Commit) {
    return NodeGit.Branch.create(
      repo,
      name,
      commit,
      force ? 1 : 0,
      signature,
      logMessage);
  }
  else {
    return repo.getCommit(commit).then(function(commit) {
      return NodeGit.Branch.create(
        repo,
        name,
        commit,
        force ? 1 : 0,
        signature,
        logMessage);
    });
  }
};

/**
 * Look up a refs's commit.
 *
 * @param {String|Ref} name Ref name, e.g. "master", "refs/heads/master"
 *                          or Branch Ref
 * @param {Function} callback
 * @return {Commit}
 */
Repository.prototype.getReferenceCommit = function(name, callback) {
  var repository = this;

  return this.getReference(name).then(function(reference) {
    return repository.getCommit(reference.target()).then(function(commit) {
      if (typeof callback === "function") {
        callback(null, commit);
      }

      return commit;
    });
  }, callback);
};

/**
* Look up a branch. Alias for `getReference`
*
* @param {String|Ref} name Ref name, e.g. "master", "refs/heads/master"
*                          or Branch Ref
* @param {Function} callback
* @return {Ref}
*/
Repository.prototype.getBranch = function(name, callback) {
  return this.getReference(name, callback);
};

/**
* Look up a branch's most recent commit. Alias to `getReferenceCommit`
*
* @param {String|Ref} name Ref name, e.g. "master", "refs/heads/master"
*                          or Branch Ref
* @param {Function} callback
* @return {Commit}
*/
Repository.prototype.getBranchCommit = function(name, callback) {
  return this.getReferenceCommit(name, callback);
};

/**
 * Gets the branch that HEAD currently points to
 * Is an alias to head()
 * @return {Reference}
 */
Repository.prototype.getCurrentBranch = function() {
  return this.head();
};

/**
 * Lookup the reference with the given name.
 *
 * @param {String|Ref} name Ref name, e.g. "master", "refs/heads/master"
 *                          or Branch Ref
 * @param {Function} callback
 * @return {Reference}
 */
Repository.prototype.getReference = function(name, callback) {
  var repository = this;

  return Reference.dwim(this, name).then(function(reference) {
    if (reference.isSymbolic()) {
      return reference.resolve().then(function(reference) {
        reference.repo = repository;

        if (typeof callback === "function") {
          callback(null, reference);
        }

        return reference;
      }, callback);
    } else {
      reference.repo = repository;
      if (typeof callback === "function") {
        callback(null, reference);
      }
      return reference;
    }
  }, callback);
};

Repository.getReferences = function(repo, type, refNamesOnly, callback) {
  return Reference.list(repo).then(function(refList) {
    var refFilterPromises = [];
    var filteredRefs = [];

    refList.forEach(function(refName) {
      refFilterPromises.push(Reference.lookup(repo, refName)
        .then(function(ref) {
          if (type == Reference.TYPE.ALL || ref.type() == type) {
            if (refNamesOnly) {
              filteredRefs.push(refName);
              return;
            }

            if (ref.isSymbolic()) {
              return ref.resolve().then(function(resolvedRef) {
                resolvedRef.repo = repo;

                filteredRefs.push(resolvedRef);
              });
            }
            else {
              filteredRefs.push(ref);
            }
          }
        })
      );
    });

    return Promise.all(refFilterPromises).then(function() {
      if (typeof callback === "function") {
        callback(null, filteredRefs);
      }
      return filteredRefs;
    }, callback);
  });
};

Repository.prototype.getReferences = function(type, callback) {
  return Repository.getReferences(this, type, false, callback);
};

Repository.prototype.getReferenceNames = function(type, callback) {
  return Repository.getReferences(this, type, true, callback);
};

/**
 * Retrieve the commit identified by oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Function} callback
 * @return {Commit}
 */
Repository.prototype.getCommit = function(oid, callback) {
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
  var repository = this;

  return Tag.lookup(repository, oid).then(function(reference) {
    reference.repo = repository;

    if (typeof callback === "function") {
      callback(null, reference);
    }

    return reference;
  }, callback);
};

/**
 * Retrieve the tag represented by the tag name.
 *
 * @param {String} Short or full tag name
 * @param {Function} callback
 * @return {Tag}
 */
Repository.prototype.getTagByName = function(name, callback) {
  var repo = this;

  name = ~name.indexOf("refs/tags/") ? name : "refs/tags/" + name;

  return Reference.nameToId(repo, name).then(function(oid) {
    return Tag.lookup(repo, oid).then(function(reference) {
      reference.repo = repo;

      if (typeof callback === "function") {
        callback(null, reference);
      }

      return reference;
    });
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
  var revWalk = Revwalk.create(this);
  revWalk.repo = this;
  return revWalk;
};

/**
 * Retrieve the master branch commit.
 *
 * @param {Function} callback
 * @return {Commit}
 */
Repository.prototype.getMasterCommit = function(callback) {
  return this.getBranchCommit("master", callback);
};

/**
 * Retrieve the commit that HEAD is currently pointing to
 *
 * @param {Function} callback
 * @return {Commit}
 */
Repository.prototype.getHeadCommit = function(callback) {
  var repo = this;

  return Reference.nameToId(repo, "HEAD").then(function(head) {
    return repo.getCommit(head, callback);
  });
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

  var repo = this;
  var promises = [];

  parents = parents || [];

  promises.push(repo.getTree(tree));

  parents.forEach(function(parent) {
    promises.push(repo.getCommit(parent));
  });

  return Promise.all(promises).then(function(results) {
    tree = results[0];

    // Get the normalized values for our input into the function
    var parentsLength = parents.length;
    parents = [];

    for (var i = 0; i < parentsLength; i++) {
      parents.push(results[i + 1]);
    }

    return Commit.create(
      repo,
      updateRef,
      author,
      committer,
      null /* use default message encoding */,
      message,
      tree,
      parents.length,
      parents
    );
  }).then(function(commit) {
    if (typeof callback === "function") {
      callback(null, commit);
    }

    return commit;
  }, callback);
};

/**
 * Creates a new commit on HEAD from the list of passed in files
 * @param {Array} filesToAdd
 * @param {Signature} author
 * @param {Signature} committer
 * @param {String} message
 * @param {Function} callback
 * @return {Oid} The oid of the new commit
 */
Repository.prototype.createCommitOnHead = function(
  filesToAdd,
  author,
  committer,
  message,
  callback){
  var repo = this;

  return repo.openIndex().then(function(index) {
    index.read(true);

    filesToAdd.forEach(function(filePath) {
      index.addByPath(filePath);
    });

    index.write();

    return index.writeTree();
  }).then(function(treeOid) {

    return repo.getHeadCommit().then(function(parent) {
      return repo.createCommit(
        "HEAD",
        author,
        committer,
        message,
        treeOid,
        [parent],
        callback);
    });
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

/**
 * Gets the default signature for the default user and now timestamp
 * @return {Signature}
 */
Repository.prototype.defaultSignature = function() {
  return NodeGit.Signature.default(this);
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
 * Gets a remote from the repo
 *
 * @param {String|Remote} remote
 * @param {Function} callback
 * @return {Remote} The remote object
 */
Repository.prototype.getRemote = function(remote, callback) {
  if (remote instanceof NodeGit.Remote) {
    return Promise.resolve(remote).then(function(remoteObj) {
      if (typeof callback === "function") {
        callback(null, remoteObj);
      }

      return remoteObj;
    }, callback);
  }

  return NodeGit.Remote.load(this, remote).then(function(remoteObj) {
    if (typeof callback === "function") {
      callback(null, remoteObj);
    }

    return remoteObj;
  }, callback);
};

/**
 * Fetches from a remote
 *
 * @param {String|Remote} remote
 */
Repository.prototype.fetch = function(
  remote,
  remoteCallbacks,
  ignoreCertErrors,
  callback)
{
  var repo = this;

  return repo.getRemote(remote).then(function(remote) {
    remote.setCallbacks(remoteCallbacks);
    remote.checkCert(ignoreCertErrors ? 0 : 1);

    return remote.fetch(repo.defaultSignature(), "Fetch from " + remote)
    .then(function() {
      if (typeof callback === "function") {
        callback();
      }
    });
  }, callback);
};

/**
 * Fetches from all remotes
 */
Repository.prototype.fetchAll = function(
  remoteCallbacks,
  ignoreCertErrors,
  callback)
{
  var repo = this;

  return repo.getRemotes().then(function(remotes) {
    var fetchPromises = [];

    remotes.forEach(function(remote) {
      fetchPromises.push(
        repo.fetch(remote, remoteCallbacks, ignoreCertErrors, callback));
    });

    return Promise.all(fetchPromises);
  }, callback);
};

/**
 * Merge a branch onto another branch
 *
 * @param {String|Ref}  from
 * @param {String|Ref}  to
 * @return {Oid|Index}  A commit id for a succesful merge or an index for a
 *                      merge with conflicts
 */
Repository.prototype.mergeBranches = function(to, from, signature) {
  var repo = this;
  var fromBranch;
  var toBranch;

  signature = signature || repo.defaultSignature();

  return Promise.all([
    repo.getBranch(to),
    repo.getBranch(from)
  ]).then(function(branches) {
    toBranch = branches[0];
    fromBranch = branches[1];

    return Promise.all([
      repo.getBranchCommit(toBranch),
      repo.getBranchCommit(fromBranch)
    ]);
  }).then(function(branchCommits) {
    var toCommitOid = branchCommits[0].toString();
    var fromCommitOid = branchCommits[1].toString();

    return NodeGit.Merge.base(repo, toCommitOid, fromCommitOid)
    .then(function(baseCommit) {
      if (baseCommit.toString() == fromCommitOid) {
        // The commit we're merging to is already in our history.
        // nothing to do so just return the commit the branch is on
        return toCommitOid;
      }
      else if (baseCommit.toString() == toCommitOid) {
        // fast forward
        var message =
          "Fast forward branch " +
          toBranch.shorthand() +
          " to branch " +
          fromBranch.shorthand();

        return toBranch.setTarget(
          fromCommitOid,
          signature,
          message)
        .then(function() {
          return fromCommitOid;
        });
      }
      else {
        // We have to merge. Lets do it!
        return NodeGit.Merge.commits(repo, toCommitOid, fromCommitOid)
        .then(function(index) {
          // if we have conflicts then throw the index
          if (index.hasConflicts()) {
            throw index;
          }

          // No conflicts so just go ahead with the merge
          index.write();
          return index.writeTreeTo(repo);
        }).then(function(oid) {
          var message =
            "Merged " +
            fromBranch.shorthand() +
            " into " +
            toBranch.shorthand();

          return repo.createCommit(
            toBranch.name(),
            signature,
            signature,
            message,
            oid,
            [toCommitOid, fromCommitOid]);
        });
      }
    });
  });
};

// Override Repository.initExt to normalize initoptions
var initExt = Repository.initExt;
Repository.initExt = function(repo_path, opts) {
  opts = normalizeOptions(opts, NodeGit.RepositoryInitOptions);
  return initExt(repo_path, opts);
};


module.exports = Repository;
