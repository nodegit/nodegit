var Promise = require("nodegit-promise");
var NodeGit = require("../");
var Blob = NodeGit.Blob;
var Checkout = NodeGit.Checkout;
var Commit = NodeGit.Commit;
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var Reference = NodeGit.Reference;
var Remote = NodeGit.Remote;
var Repository = NodeGit.Repository;
var Revwalk = NodeGit.Revwalk;
var Status = NodeGit.Status;
var StatusFile = NodeGit.StatusFile;
var StatusList = NodeGit.StatusList;
var Tag = NodeGit.Tag;
var Tree = NodeGit.Tree;
var TreeBuilder = NodeGit.Treebuilder;

Object.defineProperty(Repository.prototype, "openIndex", {
  enumerable: false,
  value: Repository.prototype.index
});

/**
 * Creates a branch with the passed in name pointing to the commit
 *
 * @async
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
 * @async
 * @param {String|Ref} name Ref name, e.g. "master", "refs/heads/master"
 *                          or Branch Ref
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
* @async
* @param {String|Ref} name Ref name, e.g. "master", "refs/heads/master"
*                          or Branch Ref
* @return {Ref}
*/
Repository.prototype.getBranch = function(name, callback) {
  return this.getReference(name, callback);
};

/**
* Look up a branch's most recent commit. Alias to `getReferenceCommit`
*
* @async
* @param {String|Ref} name Ref name, e.g. "master", "refs/heads/master"
*                          or Branch Ref
* @return {Commit}
*/
Repository.prototype.getBranchCommit = function(name, callback) {
  return this.getReferenceCommit(name, callback);
};

/**
 * Gets the branch that HEAD currently points to
 * Is an alias to head()
 * @async
 * @return {Reference}
 */
Repository.prototype.getCurrentBranch = function() {
  return this.head();
};

/**
 * Lookup the reference with the given name.
 *
 * @async
 * @param {String|Ref} name Ref name, e.g. "master", "refs/heads/master"
 *                          or Branch Ref
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


/**
 * Lookup references for a repository.
 *
 * @async
 * @param {Reference.TYPE} type Type of reference to look up
 * @return {Array<Reference>}
 */
Repository.prototype.getReferences = function(type, callback) {
  return Repository.getReferences(this, type, false, callback);
};

/**
 * Lookup reference names for a repository.
 *
 * @async
 * @param {Reference.TYPE} type Type of reference to look up
 * @return {Array<String>}
 */
Repository.prototype.getReferenceNames = function(type, callback) {
  return Repository.getReferences(this, type, true, callback);
};

/**
 * Retrieve the commit identified by oid.
 *
 * @async
 * @param {String|Oid} String sha or Oid
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
 * @async
 * @param {String|Oid} String sha or Oid
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
 * @async
 * @param {String|Oid} String sha or Oid
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
 * Creates a new annotated tag
 *
 * @async
 * @param {String|Oid} String sha or Oid
 * @param {String} name the name of the tag
 * @param {String} message the description that will be attached to the
 * annotated tag
 * @return {Tag}
 */
Repository.prototype.createTag = function(oid, name, message, callback) {
  var repository = this;
  var signature = repository.defaultSignature();

  return Commit.lookup(repository, oid)
    .then(function(commit) {
      // Final argument is `force` which overwrites any previous tag
      return Tag.create(repository, name, commit, signature, message, 0);
    })
    .then(function(tagOid) {
      return repository.getTag(tagOid, callback);
    });
};

/**
 * Creates a new lightweight tag
 *
 * @async
 * @param {String|Oid} String sha or Oid
 * @param {String} name the name of the tag
 * @return {Reference}
 */
Repository.prototype.createLightweightTag = function(oid, name, callback) {
  var repository = this;

  return Commit.lookup(repository, oid)
    .then(function(commit) {
      // Final argument is `force` which overwrites any previous tag
      return Tag.createLightweight(repository, name, commit, 0);
    })
    .then(function() {
      return Reference.lookup(repository, "refs/tags/" + name);
    });
};

/**
 * Retrieve the tag represented by the oid.
 *
 * @async
 * @param {String|Oid} String sha or Oid
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
 * @async
 * @param {String} Short or full tag name
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
 * Deletes a tag from a repository by the tag name.
 *
 * @async
 * @param {String} Short or full tag name
 */
Repository.prototype.deleteTagByName = function(name) {
  var repository = this;

  name = ~name.indexOf("refs/tags/") ? name.substr(10) : name;

  return Tag.delete(repository, name);
};

/**
 * Instantiate a new revision walker for browsing the Repository"s history.
 * See also `Commit.prototype.history()`
 *
 * @param {String|Oid} String sha or Oid
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
 * @async
 * @return {Commit}
 */
Repository.prototype.getMasterCommit = function(callback) {
  return this.getBranchCommit("master", callback);
};

/**
 * Retrieve the commit that HEAD is currently pointing to
 *
 * @async
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
 * @async
 * @param {String} updateRef
 * @param {Signature} author
 * @param {Signature} committer
 * @param {String} message
 * @param {Tree|Oid|String} Tree
 * @param {Array} parents
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
 * @async
 * @param {Array} filesToAdd
 * @param {Signature} author
 * @param {Signature} committer
 * @param {String} message
 * @return {Oid} The oid of the new commit
 */
Repository.prototype.createCommitOnHead = function(
  filesToAdd,
  author,
  committer,
  message,
  callback){
  var repo = this;
  var index;

  return repo.openIndex().then(function(index_) {
    index = index_;
    index.read(1);
    return index.addAll();
  })
  .then(function() {
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
 * @return {Blob}
 */
Repository.prototype.createBlobFromBuffer = function(buffer, callback) {
  return Blob.createFromBuffer(this, buffer, buffer.length, callback);
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
  var result = NodeGit.Signature.default(this);

  if (!result || !result.name()) {
    result = NodeGit.Signature.now("unknown", "unknown@unknown.com");
  }

  return result;
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

  return NodeGit.Remote.lookup(this, remote).then(function(remoteObj) {
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
 * @param {Object|RemoteCallback} remoteCallbacks Any custom callbacks needed
 * @param {Bool} pruneAfter will perform a prune after the fetch if true
 */
Repository.prototype.fetch = function(
  remote,
  remoteCallbacks,
  pruneAfter,
  callback)
{
  var repo = this;

  return repo.getRemote(remote).then(function(remote) {
    remote.setCallbacks(remoteCallbacks);

    return remote.fetch(null, repo.defaultSignature(), "Fetch from " + remote)
    .then(function() {
      if (pruneAfter) {
        remote.prune();
      }
    })
    .then(function() {
      return remote.disconnect();
    }).then(function() {
      if (typeof callback === "function") {
        callback();
      }
    });
  }, callback);
};

/**
 * Fetches from all remotes
 * @param {Object|RemoteCallback} remoteCallbacks Any custom callbacks needed
 * @param {Bool} pruneAfter will perform a prune after the fetch if true
 */
Repository.prototype.fetchAll = function(
  remoteCallbacks,
  pruneAfter,
  callback)
{
  var repo = this;

  return repo.getRemotes().then(function(remotes) {
    var fetchPromises = [];

    remotes.forEach(function(remote) {
      fetchPromises.push(
        repo.fetch(remote, remoteCallbacks, pruneAfter, callback));
    });

    return Promise.all(fetchPromises);
  }, callback);
};

/**
 * Merge a branch onto another branch
 *
 * @param {String|Ref}  to
 * @param {String|Ref}  from
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
  ]).then(function(objects) {
    toBranch = objects[0];
    fromBranch = objects[1];

    return Promise.all([
      repo.getBranchCommit(toBranch),
      repo.getBranchCommit(fromBranch)
    ]);
  })
  .then(function(branchCommits) {
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

        return branchCommits[1].getTree()
        .then(function(tree) {
          if (toBranch.isHead()) {
            // Checkout the tree if we're on the branch
            var opts = {
              checkoutStrategy: NodeGit.Checkout.STRATEGY.SAFE_CREATE
            };
            return NodeGit.Checkout.tree(repo, tree, opts);
          }
        })
        .then(function() {
          return toBranch.setTarget(
            fromCommitOid,
            signature,
            message)
          .then(function() {
            return fromCommitOid;
          });
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

/**
 * Goes through a rebase's rebase operations and commits them if there are
 * no merge conflicts
 *
 * @param {Repository}  repository  The repository that the rebase is being
 *                                  performed in
 * @param {Rebase}      rebase      The current rebase being performed
 * @param {Signature}   signature   Identity of the one performing the rebase
 * @return  {Int|Index} An error code for an unsuccesful rebase or an index for
 *                      a rebase with conflicts
 */
function performRebase(repository, rebase, signature) {
  return rebase.next()
    .then(function(rebaseOperation) {
      return repository.openIndex()
        .then(function(index) {
          if (index.hasConflicts()) {
            throw index;
          }

          if (rebaseOperation) {
            rebase.commit(null, signature);

            return performRebase(repository, rebase, signature);
          }

          return rebase.finish(signature);
        });
    });
}

/**
 * Rebases a branch onto another branch
 *
 * @param {String}    branch
 * @param {String}    upstream
 * @param {String}    onto
 * @param {Signature} signature Identity of the one performing the rebase
 * @return {Oid|Index}  A commit id for a succesful merge or an index for a
 *                      rebase with conflicts
 */
Repository.prototype.rebaseBranches = function(
  branch,
  upstream,
  onto,
  signature)
{
  var repo = this;

  signature = signature || repo.defaultSignature();

  return Promise.all([
    repo.getReference(branch),
    upstream ? repo.getReference(upstream) : null,
    onto ? repo.getReference(upstream) : null
  ])
  .then(function(refs) {
    return Promise.all([
      NodeGit.AnnotatedCommit.fromRef(repo, refs[0]),
      upstream ? NodeGit.AnnotatedCommit.fromRef(repo, refs[1]) : null,
      onto ? NodeGit.AnnotatedCommit.fromRef(repo, refs[2]) : null
    ]);
  })
  .then(function(annotatedCommits) {
    return NodeGit.Rebase.init(repo, annotatedCommits[0], annotatedCommits[1],
      annotatedCommits[2], signature, null);
  })
  .then(function(rebase) {
    return performRebase(repo, rebase, signature);
  })
  .then(function(error) {
    if (error) {
      throw error;
    }

    return repo.getBranchCommit("HEAD");
  });
};

/**
 * Continues an existing rebase
 *
 * @param {Signature} signature Identity of the one performing the rebase
 * @return {Oid|Index}  A commit id for a succesful merge or an index for a
 *                      rebase with conflicts
 */
Repository.prototype.continueRebase = function(signature) {
  var repo = this;

  return repo.openIndex()
    .then(function(index) {
      if (index.hasConflicts()) {
        throw index;
      }

      return NodeGit.Rebase.open(repo);
    })
    .then(function(rebase) {
      rebase.commit(null, signature);

      return performRebase(repo, rebase, signature);
    })
    .then(function(error) {
      if (error) {
        throw error;
      }

      return repo.getBranchCommit("HEAD");
    });
};

// Override Repository.initExt to normalize initoptions
var initExt = Repository.initExt;
Repository.initExt = function(repo_path, opts) {
  opts = normalizeOptions(opts, NodeGit.RepositoryInitOptions);
  return initExt(repo_path, opts);
};

/**
 * Get the status of a repo to it's working directory
 *
 * @param {obj} opts
 * @return {Array<StatusFile>}
 */
Repository.prototype.getStatus = function(opts) {
  var statuses = [];
  var statusCallback = function(path, status) {
    statuses.push(new StatusFile({path: path, status: status}));
  };

  if (!opts) {
    opts = {
      flags: Status.OPT.INCLUDE_UNTRACKED |
             Status.OPT.RECURSE_UNTRACKED_DIRS
    };
  }

  return Status.foreachExt(this, opts, statusCallback).then(function() {
    return statuses;
  });
};

/**
 * Get extended statuses of a repo to it's working directory. Status entries
 * have `status`, `headToIndex` delta, and `indexToWorkdir` deltas
 *
 * @param {obj} opts
 * @return {Array<StatusEntry>}
 */
Repository.prototype.getStatusExt = function(opts) {
  var statuses = [];

  if (!opts) {
    opts = {
      flags: Status.OPT.INCLUDE_UNTRACKED |
             Status.OPT.RECURSE_UNTRACKED_DIRS |
             Status.OPT.RENAMES_INDEX_TO_WORKDIR |
             Status.OPT.RENAMES_HEAD_TO_INDEX |
             Status.OPT.RENAMES_FROM_REWRITES
    };
  }

  var list = StatusList.create(this, opts);

  for (var i = 0; i < list.entrycount(); i++) {
    var entry = Status.byIndex(list, i);
    statuses.push(new StatusFile({entry: entry}));
  }

  return statuses;
};

/**
 * This will set the HEAD to point to the local branch and then attempt
 * to update the index and working tree to match the content of the
 * latest commit on that branch
 * @param {String|Reference} branch the branch to checkout
 * @param {Object|CheckoutOptions} opts the options to use for the checkout
 */
Repository.prototype.checkoutBranch = function(branch, opts) {
  var repo = this;
  var reference;
  opts = opts || {};
  opts.checkoutStrategy = opts.checkoutStrategy ||
    Checkout.STRATEGY.SAFE_CREATE;
  return repo.getReference(branch)
  .then(function(ref) {
    if (!ref.isBranch()) {
      return false;
    }
    reference = ref;
    return repo.getBranchCommit(ref.name());
  })
  .then(function(commit) {
    return commit.getTree();
  })
  .then(function(tree) {
    return Checkout.tree(repo, tree, opts);
  })
  .then(function() {
    var name = reference.name();
    return repo.setHead(name,
      repo.defaultSignature(),
      "Switch HEAD to " + name);
  });
};

var fetchheadForeach = Repository.prototype.fetchheadForeach;
/**
 * @async
 * @param {FetchheadForeachCb} callback The callback function to be called on
 * each entry
 */
Repository.prototype.fetchheadForeach = function(callback) {
  return fetchheadForeach.call(this, callback, null);
};

module.exports = Repository;
