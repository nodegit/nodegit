var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));
var NodeGit = require("../");
var Blob = NodeGit.Blob;
var Checkout = NodeGit.Checkout;
var Commit = NodeGit.Commit;
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var shallowClone = NodeGit.Utils.shallowClone;
var path = require("path");
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
function(name, commit, force) {
  var repo = this;

  if (commit instanceof Commit) {
    return NodeGit.Branch.create(
      repo,
      name,
      commit,
      force ? 1 : 0);
  }
  else {
    return repo.getCommit(commit).then(function(commit) {
      return NodeGit.Branch.create(
        repo,
        name,
        commit,
        force ? 1 : 0);
    });
  }
};

/**
 * Creates a branch with the passed in name pointing to the commit
 *
 * @async
 * @param {String} startPath The base path where the lookup starts.
 * @param {Number} acrossFs If non-zero, then the lookup will not stop when a
                            filesystem device change is detected while exploring
                            parent directories.
 * @param {String} ceilingDirs A list of absolute symbolic link free paths.
                               which
 * @return {String} Path of the git repository
 */
var discover = Repository.discover;
Repository.discover = function(searchPath, acrossFs, ceilingDirs, callback) {
  return discover(searchPath, acrossFs, ceilingDirs)
    .then(function(foundPath) {
      foundPath = path.resolve(foundPath);
      if (typeof callback === "function") {
        callback(null, foundPath);
      }
      return foundPath;
    }, callback);
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
          if (type == Reference.TYPE.LISTALL || ref.type() == type) {
            if (refNamesOnly) {
              filteredRefs.push(refName);
              return;
            }

            if (ref.isSymbolic()) {
              return ref.resolve().then(function(resolvedRef) {
                resolvedRef.repo = repo;

                filteredRefs.push(resolvedRef);
              })
              .catch(function() {
                // If we can't resolve the ref then just ignore it.
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

  return Reference.nameToId(repo, "HEAD")
    .then(function(head) {
      return repo.getCommit(head, callback);
    })
    .catch(function() {
      return null;
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
  callback) {

  var index;
  var repo = this;

  return repo.openIndex()
    .then(function(index_) {
      index = index_;
      index.read(1);
      if (!filesToAdd) { filesToAdd = []; }
      filesToAdd.forEach(function(filePath) {
        index.addByPath(filePath);
      });
      index.write();
      return index.writeTree();
    })
    .then(function(treeOid) {
      return repo.getHeadCommit()
        .then(function(parent) {
          return repo.createCommit(
            "HEAD",
            author,
            committer,
            message,
            treeOid,
            [parent]
          );
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
 * @param {Object|FetchOptions} fetchOptions Options for the fetch, includes
 *                                           callbacks for fetching
 */
Repository.prototype.fetch = function(
  remote,
  fetchOptions,
  callback)
{
  var repo = this;

  function finallyFn() {
    if (typeof callback === "function") {
      callback();
    }
  }

  return repo.getRemote(remote)
    .then(function(remote) {
      return remote.fetch(null, fetchOptions, "Fetch from " + remote)
      .then(function() {
        return remote.disconnect();
      });
    })
    .then(finallyFn, finallyFn);
};

/**
 * Fetches from all remotes. This is done in series due to deadlocking issues
 * with fetching from many remotes that can happen.
 * @param {Object|FetchOptions} fetchOptions Options for the fetch, includes
 *                                           callbacks for fetching
 * @param {Function} callback
 */
Repository.prototype.fetchAll = function(
  fetchOptions,
  callback)
{
  var repo = this;

  function createCallbackWrapper(fn, remote) {
    return function() {
      var args = Array.prototype.slice.call(arguments);
      args.push(remote);

      return fn.apply(this, args);
    }.bind(this);
  }

  fetchOptions = fetchOptions || {};

  var remoteCallbacks = fetchOptions.callbacks || {};

  var credentials = remoteCallbacks.credentials;
  var certificateCheck = remoteCallbacks.certificateCheck;
  var transferProgress = remoteCallbacks.transferProgress;

  return repo.getRemotes()
    .then(function(remotes) {
      return remotes.reduce(function(fetchPromise, remote) {
        var wrappedFetchOptions = shallowClone(fetchOptions);
        var wrappedRemoteCallbacks = shallowClone(remoteCallbacks);

        if (credentials) {
          wrappedRemoteCallbacks.credentials =
            createCallbackWrapper(credentials, remote);
        }

        if (certificateCheck) {
          wrappedRemoteCallbacks.certificateCheck =
            createCallbackWrapper(certificateCheck, remote);
        }

        if (transferProgress) {
          wrappedRemoteCallbacks.transferProgress =
            createCallbackWrapper(transferProgress, remote);
        }

        wrappedFetchOptions.callbacks = wrappedRemoteCallbacks;

        return fetchPromise.then(function() {
          return repo.fetch(remote, wrappedFetchOptions);
        });
      }, Promise.resolve());
    })
    .then(function() {
      if (typeof callback === "function") {
        callback();
      }
    });
};


/**
 * Merge a branch onto another branch
 *
 * @param {String|Ref}        to
 * @param {String|Ref}        from
 * @param {Signature}         signature
 * @param {Merge.PREFERENCE}  mergePreference
 * @return {Oid|Index}  A commit id for a succesful merge or an index for a
 *                      merge with conflicts
 */
Repository.prototype.mergeBranches =
  function(to, from, signature, mergePreference) {
  var repo = this;
  var fromBranch;
  var toBranch;
  mergePreference = mergePreference || NodeGit.Merge.PREFERENCE.NONE;

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
      else if (baseCommit.toString() == toCommitOid &&
                mergePreference !== NodeGit.Merge.PREFERENCE.NO_FASTFORWARD) {
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
              checkoutStrategy: NodeGit.Checkout.STRATEGY.SAFE |
                NodeGit.Checkout.STRATEGY.RECREATE_MISSING
            };
            return NodeGit.Checkout.tree(repo, tree, opts);
          }
        })
        .then(function() {
          return toBranch.setTarget(
            fromCommitOid,
            message)
          .then(function() {
            return fromCommitOid;
          });
        });
      }
      else if (mergePreference !== NodeGit.Merge.PREFERENCE.FASTFORWARD_ONLY) {
        var updateHead;
        // We have to merge. Lets do it!
        return NodeGit.Reference.lookup(repo, "HEAD")
        .then(function(headRef) {
          return headRef.resolve();
        })
        .then(function(headRef) {
          updateHead = !!headRef && (headRef.name() === toBranch.name());
          return NodeGit.Merge.commits(repo, toCommitOid, fromCommitOid);
        })
        .then(function(index) {
          // if we have conflicts then throw the index
          if (index.hasConflicts()) {
            throw index;
          }

          // No conflicts so just go ahead with the merge
          index.write();
          return index.writeTreeTo(repo);
        })
        .then(function(oid) {
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
        })
        .then(function(commit) {
        //   // we've updated the checked out branch, so make sure we update
        //   // head so that our index isn't messed up
          if (updateHead) {
            return repo.getBranch(to)
            .then(function(branch) {
              return repo.getBranchCommit(branch);
            })
            .then(function(branchCommit) {
              return branchCommit.getTree();
            })
            .then(function(tree) {
              var opts = {
                checkoutStrategy: NodeGit.Checkout.STRATEGY.SAFE |
                  NodeGit.Checkout.STRATEGY.RECREATE_MISSING
              };
              return NodeGit.Checkout.tree(repo, tree, opts);
            })
            .then(function() {
              return commit;
            });
          }
          else {
            return commit;
          }
        });
      }
      else {
        // A non fast-forwardable merge with ff-only
        return toCommitOid;
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
  var branchCommit;
  var upstreamCommit;
  var ontoCommit;

  signature = signature || repo.defaultSignature();

  return Promise.all([
    repo.getReference(branch),
    upstream ? repo.getReference(upstream) : null,
    onto ? repo.getReference(onto) : null
  ])
  .then(function(refs) {
    return Promise.all([
      NodeGit.AnnotatedCommit.fromRef(repo, refs[0]),
      upstream ? NodeGit.AnnotatedCommit.fromRef(repo, refs[1]) : null,
      onto ? NodeGit.AnnotatedCommit.fromRef(repo, refs[2]) : null
    ]);
  })
  .then(function(annotatedCommits) {
    branchCommit = annotatedCommits[0];
    upstreamCommit = annotatedCommits[1];
    ontoCommit = annotatedCommits[2];

    return NodeGit.Merge.base(repo, branchCommit.id(), upstreamCommit.id());
  })
  .then(function(oid) {
    if (oid.toString() === branchCommit.id().toString()) {
      // we just need to fast-forward
      return repo.mergeBranches(branch, upstream)
        .then(function() {
          // checkout 'branch' to match the behavior of rebase
          return repo.checkoutBranch(branch);
        });
    } else if (oid.toString() === upstreamCommit.id().toString()) {
      // 'branch' is already on top of 'upstream'
      // checkout 'branch' to match the behavior of rebase
      return repo.checkoutBranch(branch);
    }

    return NodeGit.Rebase.init(repo, branchCommit, upstreamCommit, ontoCommit)
      .then(function(rebase) {
        return performRebase(repo, rebase, signature);
      })
      .then(function(error) {
        if (error) {
          throw error;
        }
      });
  })
  .then(function() {
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

  signature = signature || repo.defaultSignature();

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

  return StatusList.create(this, opts)
    .then(function(list) {
      for (var i = 0; i < list.entrycount(); i++) {
        var entry = Status.byIndex(list, i);
        statuses.push(new StatusFile({entry: entry}));
      }

      return statuses;
    });
};

/**
 * This will set the HEAD to point to the local branch and then attempt
 * to update the index and working tree to match the content of the
 * latest commit on that branch
 *
 * @async
 * @param {String|Reference} branch the branch to checkout
 * @param {Object|CheckoutOptions} opts the options to use for the checkout
 */
Repository.prototype.checkoutBranch = function(branch, opts) {
  var repo = this;
  var reference;
  opts = opts || {};

  opts.checkoutStrategy = opts.checkoutStrategy ||
    (NodeGit.Checkout.STRATEGY.SAFE |
      NodeGit.Checkout.STRATEGY.RECREATE_MISSING);
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
    return repo.setHead(name);
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

/**
 * Stages or unstages line selection of a specified file
 * @async
 * @param {String} filePath The relative path of this file in the repo
 * @param {Array} newLines The array of DiffLine objects
 *                            selected for staging or unstaging
 * @param {Boolean} isStaged Are the selected lines currently staged
 * @return {Number} 0 or an error code
 */
Repository.prototype.stageLines =
          function(filePath, selectedLines, isSelectionStaged) {

  function applySelectedLinesToBlob
              (pathHunks, isStaged, newLines, originalBlob) {
    var lineTypes = {
      ADDED: 43, // ascii code for '+'
      DELETED: 45 // ascii code for '-'
    };
    var newContent = "";
    var oldIndex = 0;
    var linesPromises = [];

    //split the original file into lines
    var oldLines = originalBlob.toString().split("\n");

    //if no selected lines were sent, return the original content
    if (!newLines || newLines.length === 0) {
      return originalBlob;
    }

    function lineEqualsFirstNewLine(hunkLine) {
      return ((hunkLine.oldLineno() === newLines[0].oldLineno()) &&
              (hunkLine.newLineno() === newLines[0].newLineno()));
    }

    function processSelectedLine(hunkLine) {
      //if this hunk line is a selected line find the selected line
      var newLine = newLines.filter(function(nLine) {
        return ((hunkLine.oldLineno() === nLine.oldLineno()) &&
                (hunkLine.newLineno() === nLine.newLineno()));
      });

      if (hunkLine.raw.content()
        .indexOf("\\ No newline at end of file") != -1) {
        return false;
      }

      //determine what to add to the new content
      if ((isStaged && newLine && newLine.length > 0) ||
            (!isStaged && (!newLine || newLine.length === 0))) {
        if (hunkLine.origin() !== lineTypes.ADDED) {
          newContent += hunkLine.content();
          if (hunkLine.raw.numLines() !== 0) {
            newContent += "\n";
          }
        }
        if ((isStaged && hunkLine.origin() !== lineTypes.DELETED) ||
            (!isStaged && hunkLine.origin() !== lineTypes.ADDED)) {
          oldIndex++;
        }
      }
      else {
        switch (hunkLine.origin()) {
          case lineTypes.ADDED:
            newContent += hunkLine.content();
            if (hunkLine.raw.numLines() !== 0) {
              newContent += "\n";
            }
            if (isStaged) {
              oldIndex++;
            }
            break;
          case lineTypes.DELETED:
            if (!isStaged) {
              oldIndex++;
            }
            break;
          default:
            newContent += oldLines[oldIndex++];
            if (oldIndex < oldLines.length) {
              newContent += "\n";
            }
            break;
        }
      }
    }

    //find the affected hunk
    pathHunks.forEach(function(pathHunk) {
      linesPromises.push(pathHunk.lines());
    });
    return Promise.all(linesPromises).then(function(results) {
      for (var index = 0; index < results.length &&
                    newContent.length < 1; index++) {
        var hunkStart = isStaged ? pathHunks[index].hunk.newStart()
                        : pathHunks[index].hunk.oldStart();
        var lines = results[index];
        if (lines.filter(lineEqualsFirstNewLine).length > 0) {
          //add content that is before the hunk
          while (hunkStart > (oldIndex + 1)) {
            newContent += oldLines[oldIndex++] + "\n";
          }

          //modify the lines of the hunk according to the selection
          lines.forEach(processSelectedLine);

          //add the rest of the file
          while (oldLines.length > oldIndex) {
            newContent += oldLines[oldIndex++] +
                          (oldLines.length > oldIndex ? "\n" : "");
          }
        }
      }
      return newContent;
    });
  }

  var repo = this;
  var index;
  var diffPromise = isSelectionStaged ?
    repo.getHeadCommit()
      .then(function getTreeFromCommit(commit) {
        return commit.getTree();
      })
      .then(function getDiffFromTree(tree) {
        return NodeGit.Diff.treeToIndex(repo, tree, index);
      })
    :
    NodeGit.Diff.indexToWorkdir(repo, index, {
        flags:
          NodeGit.Diff.OPTION.SHOW_UNTRACKED_CONTENT |
          NodeGit.Diff.OPTION.RECURSE_UNTRACKED_DIRS
      });

    //The following chain checks if there is a patch with no hunks left for the
    //file, and no filemode changes were done on the file. It is then safe to
    //stage the entire file so the file doesn't show as having unstaged changes
    //in `git status`. Also, check if there are no type changes.
    var lastHunkStagedPromise = function lastHunkStagedPromise(result) {
      return NodeGit.Diff.indexToWorkdir(repo, index, {
          flags:
            NodeGit.Diff.OPTION.SHOW_UNTRACKED_CONTENT |
            NodeGit.Diff.OPTION.RECURSE_UNTRACKED_DIRS
        })
      .then(function (diff) {
        return diff.patches();
      })
      .then(function(patches) {
        var pathPatch = patches.filter(function(patch) {
          return patch.newFile().path() === filePath;
        });
        var emptyPatch = false;
        if (pathPatch.length > 0) {
          //No hunks, unchanged file mode, and no type changes.
          emptyPatch = pathPatch[0].patch.numHunks() === 0 &&
            pathPatch[0].oldFile().mode() == pathPatch[0].newFile().mode() &&
            !pathPatch[0].isTypeChange();
        }
        if (emptyPatch) {
          index.addByPath(filePath);
          return index.write();
        } else {
          return result;
        }
      });
    };

  var indexLock = repo.path().replace(".git/", "") + ".git/index.lock";

  return fse.remove(indexLock)
    .then(function() {
      return repo.openIndex();
    })
    .then(function(indexResult) {
      index = indexResult;
      return index.read(1);
    })
    .then(function() {
      return diffPromise;
    })
    .then(function(diff) {
      if (!(NodeGit.Status.file(repo, filePath) &
          NodeGit.Status.STATUS.WT_MODIFIED) &&
          !(NodeGit.Status.file(repo, filePath) &
          NodeGit.Status.STATUS.INDEX_MODIFIED)) {
        return Promise.reject
            ("Selected staging is only available on modified files.");
      }
      return diff.patches();
    })
    .then(function(patches) {
      var pathOid = index.getByPath(filePath).id;
      var pathPatch = patches.filter(function(patch) {
        return patch.newFile().path() === filePath;
      });
      if (pathPatch.length !== 1) {
        return Promise.reject("No differences found for this file.");
      }
      return Promise.all([repo.getBlob(pathOid), pathPatch[0].hunks()]);
    })
    .then(function(results) {
      var originalBlob = results[0];
      var pathHunks = results[1];
      return applySelectedLinesToBlob(
            pathHunks, isSelectionStaged, selectedLines, originalBlob);
    })
    .then(function(newContent) {
      var newContentBuffer = new Buffer(newContent);

      var newOid = repo.createBlobFromBuffer(newContentBuffer);
      return repo.getBlob(newOid);
    })
    .then(function(newBlob) {
      var entry = index.getByPath(filePath, 0);
      entry.id = newBlob.id();
      entry.path = filePath;
      entry.fileSize = newBlob.content().length;
      entry.mode = newBlob.filemode();

      index.add(entry);
      return index.write();
    })
    .then(function(result) {
      if (isSelectionStaged) {
        return result;
      } else {
        return lastHunkStagedPromise(result);
      }
    });
};

/**
 * Returns true if the repository is in the default NONE state.
 * @return {Boolean}
 */
Repository.prototype.isDefaultState = function() {
  return this.state() === NodeGit.Repository.STATE.NONE;
};

/**
 * Returns true if the repository is in the APPLY_MAILBOX or
 * APPLY_MAILBOX_OR_REBASE state.
 * @return {Boolean}
 */
Repository.prototype.isApplyingMailbox = function() {
  var state = this.state();
  return state === NodeGit.Repository.STATE.APPLY_MAILBOX ||
         state === NodeGit.Repository.STATE.APPLY_MAILBOX_OR_REBASE;
};

/**
 * Returns true if the repository is in the BISECT state.
 * @return {Boolean}
 */
Repository.prototype.isBisecting = function() {
  return this.state() === NodeGit.Repository.STATE.BISECT;
};

/**
 * Returns true if the repository is in the CHERRYPICK state.
 * @return {Boolean}
 */
Repository.prototype.isCherrypicking = function() {
  return this.state() === NodeGit.Repository.STATE.CHERRYPICK;
};

/**
 * Returns true if the repository is in the MERGE state.
 * @return {Boolean}
 */
Repository.prototype.isMerging = function() {
  return this.state() === NodeGit.Repository.STATE.MERGE;
};

/**
 * Returns true if the repository is in the REBASE, REBASE_INTERACTIVE, or
 * REBASE_MERGE state.
 * @return {Boolean}
 */
Repository.prototype.isRebasing = function() {
  var state = this.state();
  return state === NodeGit.Repository.STATE.REBASE ||
         state === NodeGit.Repository.STATE.REBASE_INTERACTIVE ||
         state === NodeGit.Repository.STATE.REBASE_MERGE;
};

/**
 * Returns true if the repository is in the REVERT state.
 * @return {Boolean}
 */
Repository.prototype.isReverting = function() {
  return this.state() === NodeGit.Repository.STATE.REVERT;
};

module.exports = Repository;
