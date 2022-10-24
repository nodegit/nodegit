var fse = require("fs-extra");
var fp = require("lodash/fp");
var NodeGit = require("../");
var Blob = NodeGit.Blob;
var Checkout = NodeGit.Checkout;
var Commit = NodeGit.Commit;
var shallowClone = NodeGit.Utils.shallowClone;
var path = require("path");
var Filter = NodeGit.Filter;
var FilterList = NodeGit.FilterList;
var Reference = NodeGit.Reference;
var Remote = NodeGit.Remote;
var Repository = NodeGit.Repository;
var Revwalk = NodeGit.Revwalk;
var Status = NodeGit.Status;
var StatusFile = NodeGit.StatusFile;
var StatusList = NodeGit.StatusList;
var Submodule = NodeGit.Submodule;
var Tag = NodeGit.Tag;
var Tree = NodeGit.Tree;
var TreeBuilder = NodeGit.Treebuilder;

var _discover = Repository.discover;
var _fetchheadForeach = Repository.prototype.fetchheadForeach;
var _mergeheadForeach = Repository.prototype.mergeheadForeach;

function applySelectedLinesToTarget
  (originalContent, newLines, pathHunks, isStaged, reverse) {
  // 43: ascii code for '+'
  // 45: ascii code for '-'
  var lineTypes = {
    ADDED: !reverse ? 43 : 45,
    DELETED: !reverse ? 45 : 43
  };
  var newContent = "";
  var oldIndex = 0;
  var linesPromises = [];

  var oldLines = originalContent.toString().split("\n");

  // if no selected lines were sent, return the original content
  if (!newLines || newLines.length === 0) {
    return originalContent;
  }

  function lineEqualsFirstNewLine(hunkLine) {
    return ((hunkLine.oldLineno() === newLines[0].oldLineno()) &&
            (hunkLine.newLineno() === newLines[0].newLineno()));
  }

  function processSelectedLine(hunkLine) {
    // if this hunk line is a selected line find the selected line
    var newLine = newLines.filter(function(nLine) {
      return ((hunkLine.oldLineno() === nLine.oldLineno()) &&
              (hunkLine.newLineno() === nLine.newLineno()));
    });

    if (hunkLine.content().indexOf("\\ No newline at end of file") !== -1) {
      return false;
    }

    // determine what to add to the new content
    if ((isStaged && newLine && newLine.length > 0) ||
          (!isStaged && (!newLine || newLine.length === 0))) {
      if (hunkLine.origin() !== lineTypes.ADDED) {
        newContent += hunkLine.content();
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

  // find the affected hunk
  pathHunks.forEach(function(pathHunk) {
    linesPromises.push(pathHunk.lines());
  });

  return Promise.all(linesPromises).then(function(results) {
    for (var i = 0; i < results.length && newContent.length < 1; i++) {
      var hunkStart = isStaged || reverse ? pathHunks[i].newStart()
                      : pathHunks[i].oldStart();
      var lines = results[i];
      if (lines.filter(lineEqualsFirstNewLine).length > 0) {
        // add content that is before the hunk
        while (hunkStart > (oldIndex + 1)) {
          newContent += oldLines[oldIndex++] + "\n";
        }

        // modify the lines of the hunk according to the selection
        lines.forEach(processSelectedLine);

        // add the rest of the file
        while (oldLines.length > oldIndex) {
          newContent += oldLines[oldIndex++] +
                        (oldLines.length > oldIndex ? "\n" : "");
        }
      }
    }

    return newContent;
  });
}

function getPathHunks(repo, index, filePath, isStaged, additionalDiffOptions) {
  var diffOptions = additionalDiffOptions ? {
    flags: additionalDiffOptions
  } : undefined;

  return Promise.resolve()
    .then(function() {
      if (isStaged) {
        return repo.getHeadCommit()
          .then(function getTreeFromCommit(commit) {
            return commit.getTree();
          })
          .then(function getDiffFromTree(tree) {
            return NodeGit.Diff.treeToIndex(repo, tree, index, diffOptions);
          });
      }

      return NodeGit.Diff.indexToWorkdir(repo, index, {
        flags:
          NodeGit.Diff.OPTION.SHOW_UNTRACKED_CONTENT |
          NodeGit.Diff.OPTION.RECURSE_UNTRACKED_DIRS |
          (additionalDiffOptions || 0)
      });
    })
    .then(function(diff) {
      return NodeGit.Status.file(repo, filePath)
        .then(function(status) {
          if (!(status & NodeGit.Status.STATUS.WT_MODIFIED) &&
              !(status & NodeGit.Status.STATUS.INDEX_MODIFIED)) {
            return Promise.reject
              ("Selected staging is only available on modified files.");
          }
          return diff.patches();
        });
    })
    .then(function(patches) {
      var pathPatch = patches.filter(function(patch) {
        return patch.newFile().path() === filePath;
      });

      if (pathPatch.length !== 1) {
        return Promise.reject("No differences found for this file.");
      }

      return pathPatch[0].hunks();
    });
}

function getReflogMessageForCommit(commit) {
  var parentCount = commit.parentcount();
  var summary = commit.summary();
  var commitType;

  if (parentCount >= 2) {
    commitType = " (merge)";
  } else if (parentCount == 0) {
    commitType = " (initial)";
  } else {
    commitType = "";
  }

  return `commit${commitType}: ${summary}`;
}

/**
 * Goes through a rebase's rebase operations and commits them if there are
 * no merge conflicts
 *
 * @param {Repository}  repository    The repository that the rebase is being
 *                                    performed in
 * @param {Rebase}      rebase        The current rebase being performed
 * @param {Signature}   signature     Identity of the one performing the rebase
 * @param {Function}    beforeNextFn  Callback to be called before each
 *                                    invocation of next(). If the callback
 *                                    returns a promise, the next() will be
 *                                    called when the promise resolves.
 * @param {Function}   beforeFinishFn Callback called before the invocation
 *                                    of finish(). If the callback returns a
 *                                    promise, finish() will be called when the
 *                                    promise resolves. This callback will be
 *                                    provided a detailed overview of the rebase
 * @return {Int|Index} An error code for an unsuccesful rebase or an index for
 *                     a rebase with conflicts
 */
function performRebase(
  repository,
  rebase,
  signature,
  beforeNextFn,
  beforeFinishFn
) {
  var beforeNextFnResult;

  /* In the case of FF merges and a beforeFinishFn, this will fail
   * when looking for 'rewritten' so we need to handle that case.
   */
  function readRebaseMetadataFile(fileName, continueOnError) {
    return fse.readFile(
      path.join(repository.path(), "rebase-merge", fileName),
      { encoding: "utf8" }
    )
      .then(fp.trim)
      .catch(function(err) {
        if (continueOnError) {
          return null;
        }
        throw err;
      });
  }

  function calcHeadName(input) {
    return input.replace(/refs\/heads\/(.*)/, "$1");
  }

  function getPromise() {
    return rebase.next()
      .then(function() {
        return repository.refreshIndex();
      })
      .then(function(index) {
        if (index.hasConflicts()) {
          throw index;
        }

        return rebase.commit(null, signature);
      })
      .then(function() {

        return performRebase(
          repository,
          rebase,
          signature,
          beforeNextFn,
          beforeFinishFn
        );
      })
      .catch(function(error) {
        if (error && error.errno === NodeGit.Error.CODE.ITEROVER) {
          const calcRewritten = fp.cond([
            [fp.isEmpty, fp.constant(null)],
            [fp.stubTrue, fp.flow([
              fp.split("\n"),
              fp.map(fp.split(" "))
            ])]
          ]);

          const beforeFinishFnPromise = !beforeFinishFn ?
            Promise.resolve() :
            Promise.all([
              readRebaseMetadataFile("onto_name"),
              readRebaseMetadataFile("onto"),
              readRebaseMetadataFile("head-name").then(calcHeadName),
              readRebaseMetadataFile("orig-head"),
              readRebaseMetadataFile("rewritten", true).then(calcRewritten)
            ])
              .then(function([
                ontoName,
                ontoSha,
                originalHeadName,
                originalHeadSha,
                rewritten
              ]) {
                return beforeFinishFn({
                  ontoName,
                  ontoSha,
                  originalHeadName,
                  originalHeadSha,
                  rebase,
                  rewritten
                });
              });

          return beforeFinishFnPromise
            .then(function() {
              return rebase.finish(signature);
            });
        } else {
          throw error;
        }
      });
  }

  if(beforeNextFn) {
    beforeNextFnResult = beforeNextFn(rebase);
    // if beforeNextFn returns a promise, chain the promise
    return Promise.resolve(beforeNextFnResult)
      .then(getPromise);
  }

  return getPromise();
}

/**
 * Look for a git repository, returning its path.
 *
 * @async
 * @param {String} startPath The base path where the lookup starts.
 * @param {Number} acrossFs If non-zero, then the lookup will not stop when a
                            filesystem device change is detected while exploring
                            parent directories.
 * @param {String} ceilingDirs A list of absolute symbolic link free paths.
                              the search will stop if any of these paths
                              are hit. This may be set to null
 * @return {String} Path of the git repository
 */
Repository.discover = function(startPath, acrossFs, ceilingDirs) {
  return _discover(startPath, acrossFs, ceilingDirs)
    .then(function(foundPath) {
      return path.resolve(foundPath);
    });
};


Repository.getReferences = function(repo, type, refNamesOnly) {
  return repo.getReferences().then(function(refList) {
    var filteredRefList = refList.filter(function(reference) {
      return type === Reference.TYPE.ALL || reference.type === type;
    });

    if (refNamesOnly) {
      return filteredRefList.map(function(reference) {
        return reference.name();
      });
    }

    return filteredRefList;
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

  return repo.getReference(branch)
  .then(function(ref) {
    if (!ref.isBranch()) {
      return false;
    }
    return repo.checkoutRef(ref, opts);
  });
};

/**
 * This will set the HEAD to point to the reference and then attempt
 * to update the index and working tree to match the content of the
 * latest commit on that reference
 *
 * @async
 * @param {Reference} reference the reference to checkout
 * @param {Object|CheckoutOptions} opts the options to use for the checkout
 */
Repository.prototype.checkoutRef = function(reference, opts) {
  var repo = this;
  opts = opts || {};

  opts.checkoutStrategy = opts.checkoutStrategy ||
    (NodeGit.Checkout.STRATEGY.SAFE |
      NodeGit.Checkout.STRATEGY.RECREATE_MISSING);
  return repo.getReferenceCommit(reference.name())
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

/**
 * Continues an existing rebase
 *
 * @async
 * @param {Signature}  signature     Identity of the one performing the rebase
 * @param {Function}   beforeNextFn  Callback to be called before each step
 *                                   of the rebase. If the callback returns a
 *                                   promise, the rebase will resume when the
 *                                   promise resolves. The rebase object is
 *                                   is passed to the callback.
 * @param {Function}   beforeFinishFn Callback called before the invocation
 *                                    of finish(). If the callback returns a
 *                                    promise, finish() will be called when the
 *                                    promise resolves. This callback will be
 *                                    provided a detailed overview of the rebase
 * @param {RebaseOptions} rebaseOptions Options to initialize the rebase object
 *                                      with
 * @return {Oid|Index}  A commit id for a succesful merge or an index for a
 *                      rebase with conflicts
 */
Repository.prototype.continueRebase = function(
  signature,
  beforeNextFn,
  beforeFinishFn,
  rebaseOptions
) {
  const repo = this;

  let rebase;
  let promiseChain = Promise.resolve();

  if (!signature) {
    promiseChain = promiseChain
      .then(() => repo.defaultSignature())
      .then((signatureResult) => {
        signature = signatureResult;
      });
  }

  return promiseChain
    .then(() => repo.refreshIndex())
    .then((index) => {
      if (index.hasConflicts()) {
        throw index;
      }

      return NodeGit.Rebase.open(repo, rebaseOptions);
    })
    .then((_rebase) => {
      rebase = _rebase;
      return rebase.commit(null, signature)
        .catch((e) => {
          // If the first commit on continueRebase is a
          // "patch already applied" error,
          // interpret that as an explicit "skip commit"
          // and ignore the error.
          const errno = fp.get(["errno"], e);
          if (errno === NodeGit.Error.CODE.EAPPLIED) {
            return;
          }

          throw e;
        });
    })
    .then(() => {
      return performRebase(
        repo,
        rebase,
        signature,
        beforeNextFn,
        beforeFinishFn
      );
    })
    .then((error) => {
      if (error) {
        throw error;
      }

      return repo.getBranchCommit("HEAD");
    });
};

/**
 * Creates a branch with the passed in name pointing to the commit
 *
 * @async
 * @param {String} name Branch name, e.g. "master"
 * @param {Commit|String|Oid} commit The commit the branch will point to
 * @param {Boolean} force Overwrite branch if it exists
 * @return {Reference}
 */
Repository.prototype.createBranch = function(name, commit, force) {
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
 * Create a blob from a buffer
 *
 * @async
 * @param {Buffer} buffer
 * @return {Oid}
 */
Repository.prototype.createBlobFromBuffer = function(buffer) {
  return Blob.createFromBuffer(this, buffer, buffer.length);
};

/**
 * Create a commit
 *
 * @async
 * @param {String} updateRef
 * @param {Signature} author
 * @param {Signature} committer
 * @param {String} message
 * @param {Oid|String} Tree
 * @param {Array} parents
 * @return {Oid} The oid of the commit
 */
Repository.prototype.createCommit = function(
  updateRef, author, committer, message, tree, parents) {

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
  });
};

/**
 * Create a commit
 *
 * @async
 * @param {Signature} author
 * @param {Signature} committer
 * @param {String} message
 * @param {Oid|String} treeOid
 * @param {Array} parents
 * @return {String} The content of the commit object
 *                  as a string
 */
Repository.prototype.createCommitBuffer = function(
  author, committer, message, treeOid, parents) {

  const repo = this;
  const promises = (parents || [])
    .reduce(function(acc, parent) {
      acc.push(repo.getCommit(parent));
      return acc;
    }, [repo.getTree(treeOid)]);

  return Promise.all(promises)
    .then(function([tree, ...parentCommits]) {
      return Commit.createBuffer(
        repo,
        author,
        committer,
        null /* use default message encoding */,
        message,
        tree,
        parentCommits.length,
        parentCommits
      );
    });
};

/**
 * Create a commit that is digitally signed
 *
 * @async
 * @param {String} updateRef
 * @param {Signature} author
 * @param {Signature} committer
 * @param {String} message
 * @param {Tree|Oid|String} Tree
 * @param {Array} parents
 * @param {Function} onSignature Callback to be called with string to be signed
 * @return {Oid} The oid of the commit
 */
Repository.prototype.createCommitWithSignature = function(
  updateRef,
  author,
  committer,
  message,
  tree,
  parents,
  onSignature
) {

  var repo = this;
  var promises = [];
  var commitContent;
  var skippedSigning;

  parents = parents || [];

  promises.push(repo.getTree(tree));

  parents.forEach(function(parent) {
    promises.push(repo.getCommit(parent));
  });

  const createCommitPromise = Promise.all(promises).then(function(results) {
    tree = results[0];

    // Get the normalized values for our input into the function
    var parentsLength = parents.length;
    parents = [];

    for (var i = 0; i < parentsLength; i++) {
      parents.push(results[i + 1]);
    }

    return Commit.createBuffer(
      repo,
      author,
      committer,
      null /* use default message encoding */,
      message,
      tree,
      parents.length,
      parents
    );
  }).then(function(commitContentResult) {
    commitContent = commitContentResult;
    if (!commitContent.endsWith("\n")) {
      commitContent += "\n";
    }
    return onSignature(commitContent);
  }).then(function({ code, field, signedData }) {
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
          author,
          committer,
          null /* use default message encoding */,
          message,
          tree,
          parents.length,
          parents
        );
      default: {
        const error = new Error(
          "Repository.prototype.createCommitWithSignature " +
          `threw with error code ${code}`
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
          return Reference.updateTerminal(
            repo,
            updateRef,
            commitOid,
            getReflogMessageForCommit(commitResult),
            committer
          );
        })
        .then(function() {
          return commitOid;
        });
      });
};

/**
 * Creates a new commit on HEAD from the list of passed in files
 *
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
  message) {

  var repo = this;

  return repo.refreshIndex()
    .then(function(index) {
      if (!filesToAdd) {
        filesToAdd = [];
      }

      return filesToAdd
        .reduce(function(lastFilePromise, filePath) {
          return lastFilePromise
            .then(function() {
              return index.addByPath(filePath);
            });
        }, Promise.resolve())
        .then(function() {
          return index.write();
        })
        .then(function() {
          return index.writeTree();
        });
    })
    .then(function(treeOid) {
      return repo.getHeadCommit()
        .then(function(parent) {
          if (parent !== null) { // To handle a fresh repo with no commits
            parent = [parent];
          }
          return repo.createCommit(
            "HEAD",
            author,
            committer,
            message,
            treeOid,
            parent
          );
        });
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
Repository.prototype.createLightweightTag = function(oid, name) {
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
 * Instantiate a new revision walker for browsing the Repository"s history.
 * See also `Commit.prototype.history()`
 *
 * @return {Revwalk}
 */
Repository.prototype.createRevWalk = function() {
  return Revwalk.create(this);
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
Repository.prototype.createTag = function(oid, name, message) {
  const repository = this;
  let signature = null;

  return repository.defaultSignature()
    .then((signatureResult) => {
      signature = signatureResult;
      return Commit.lookup(repository, oid);
    })
    .then((commit) => {
      // Final argument is `force` which overwrites any previous tag
      return Tag.create(repository, name, commit, signature, message, 0);
    })
    .then((tagOid) => {
      return repository.getTag(tagOid);
    });
};

/**
 * Gets the default signature for the default user and now timestamp
 *
 * @async
 * @return {Signature}
 */
Repository.prototype.defaultSignature = function() {
  return NodeGit.Signature.default(this)
    .then((result) => {
      if (!result || !result.name()) {
        result = NodeGit.Signature.now("unknown", "unknown@example.com");
      }
      return result;
    })
    .catch(() => {
      return NodeGit.Signature.now("unknown", "unknown@example.com");
    });
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
 * Discard line selection of a specified file.
 * Assumes selected lines are unstaged.
 *
 * @async
 * @param {String} filePath The relative path of this file in the repo
 * @param {Array} selectedLines The array of DiffLine objects
 *                            selected for discarding
 * @return {Number} 0 or an error code
 */
Repository.prototype.discardLines =
  function(filePath, selectedLines, additionalDiffOptions) {
  var repo = this;
  var fullFilePath = path.join(repo.workdir(), filePath);
  var index;
  var originalContent;
  var filterList;

  return repo.refreshIndex()
    .then(function(indexResult) {
      index = indexResult;
      return FilterList.load(
        repo,
        null,
        filePath,
        Filter.MODE.CLEAN,
        Filter.FLAG.DEFAULT
      );
    })
    .then(function(_filterList) {
      filterList = _filterList;

      if (filterList) {
        return filterList.applyToFile(repo, filePath);
      }

      return fse.readFile(fullFilePath, "utf8");
    })
    .then(function(content) {
      originalContent = content;
      return getPathHunks(repo, index, filePath, false, additionalDiffOptions);
    })
    .then(function(hunks) {
      return applySelectedLinesToTarget(
        originalContent, selectedLines, hunks, false, true
      );
    })
    .then(function(newContent) {
      return FilterList.load(
        repo,
        null,
        filePath,
        Filter.MODE.SMUDGE,
        Filter.FLAG.DEFAULT
      )
        .then(function(_filterList) {
          filterList = _filterList;
          if (filterList) {
            /* eslint-disable */
            // We need the constructor for the check in NodeGit's C++ layer
            // to accept an object, and this seems to be a nice way to do it
            return filterList.applyToData(new String(newContent));
            /* eslint-enable */
          }

          return newContent;
        });
    })
    .then(function(filteredContent) {
      return fse.writeFile(fullFilePath, filteredContent);
    });
};

/**
 * Fetches from a remote
 *
 * @async
 * @param {String|Remote} remote
 * @param {Object|FetchOptions} fetchOptions Options for the fetch, includes
 *                                           callbacks for fetching
 */
Repository.prototype.fetch = function(
  remote,
  fetchOptions)
{
  var repo = this;

  return repo.getRemote(remote)
    .then(function(remote) {
      return remote.fetch(null, fetchOptions, "Fetch from " + remote)
      .then(function() {
        return remote.disconnect();
      });
    });
};

/**
 * Fetches from all remotes. This is done in series due to deadlocking issues
 * with fetching from many remotes that can happen.
 *
 * @async
 * @param {Object|FetchOptions} fetchOptions Options for the fetch, includes
 *                                           callbacks for fetching
 */
Repository.prototype.fetchAll = function(fetchOptions) {
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

  return repo.getRemoteNames()
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
    });
};

/**
 * @async
 * @param {FetchheadForeachCb} callback The callback function to be called on
 * each entry
 */
Repository.prototype.fetchheadForeach = function(callback) {
  return _fetchheadForeach.call(this, callback, null);
};

/**
 * Retrieve the blob represented by the oid.
 *
 * @async
 * @param {String|Oid} String sha or Oid
 * @return {Blob}
 */
Repository.prototype.getBlob = function(oid) {
  var repository = this;

  return Blob.lookup(repository, oid).then(function(blob) {
    blob.repo = repository;
    return blob;
  });
};

/**
* Look up a branch. Alias for `getReference`
*
* @async
* @param {String|Reference} name Ref name, e.g. "master", "refs/heads/master"
*                              or Branch Ref
* @return {Reference}
*/
Repository.prototype.getBranch = function(name) {
  return this.getReference(name);
};

/**
* Look up a branch's most recent commit. Alias to `getReferenceCommit`
*
* @async
* @param {String|Reference} name Ref name, e.g. "master", "refs/heads/master"
*                          or Branch Ref
* @return {Commit}
*/
Repository.prototype.getBranchCommit = function(name) {
  return this.getReferenceCommit(name);
};

/**
 * Retrieve the commit identified by oid.
 *
 * @async
 * @param {String|Oid} String sha or Oid
 * @return {Commit}
 */
Repository.prototype.getCommit = function(oid) {
  var repository = this;

  return Commit.lookup(repository, oid);
};

/**
 * Gets the branch that HEAD currently points to
 * Is an alias to head()
 *
 * @async
 * @return {Reference}
 */
Repository.prototype.getCurrentBranch = function() {
  return this.head();
};

/**
 * Retrieve the commit that HEAD is currently pointing to
 *
 * @async
 * @return {Commit}
 */
Repository.prototype.getHeadCommit = function() {
  var repo = this;

  return Reference.nameToId(repo, "HEAD")
    .then(function(head) {
      return repo.getCommit(head);
    })
    .catch(function() {
      return null;
    });
};

/**
 * Retrieve the master branch commit.
 *
 * @async
 * @return {Commit}
 */
Repository.prototype.getMasterCommit = function() {
  return this.getBranchCommit("master");
};

/**
 * Lookup the reference with the given name.
 *
 * @async
 * @param {String|Reference} name Ref name, e.g. "master", "refs/heads/master"
 *                               or Branch Ref
 * @return {Reference}
 */
Repository.prototype.getReference = function(name) {
  var repository = this;

  return Reference.dwim(this, name).then(function(reference) {
    if (reference.isSymbolic()) {
      return reference.resolve().then(function(reference) {
        reference.repo = repository;
        return reference;
      });
    }

    reference.repo = repository;
    return reference;
  });
};

/**
 * Look up a refs's commit.
 *
 * @async
 * @param {String|Reference} name Ref name, e.g. "master", "refs/heads/master"
 *                              or Branch Ref
 * @return {Commit}
 */
Repository.prototype.getReferenceCommit = function(name) {
  var repository = this;

  return this.getReference(name).then(function(reference) {
    return repository.getCommit(reference.target());
  });
};

/**
 * Lookup reference names for a repository.
 *
 * @async
 * @param {Reference.TYPE} type Type of reference to look up
 * @return {Array<String>}
 */
Repository.prototype.getReferenceNames = function(type) {
  return Repository.getReferences(this, type, true);
};

/**
 * Lookup references for a repository.
 *
 * @async
 * @param {Reference.TYPE} type Type of reference to look up
 * @return {Array<Reference>}
 */

/**
 * Gets a remote from the repo
 *
 * @async
 * @param {String|Remote} remote
 * @return {Remote} The remote object
 */
Repository.prototype.getRemote = function(remote) {
  if (remote instanceof NodeGit.Remote) {
    return Promise.resolve(remote);
  }

  return NodeGit.Remote.lookup(this, remote);
};

/**
* Lists out the remotes in the given repository.
*
* @async
* @return {Object} Promise object.
*/
Repository.prototype.getRemoteNames = function() {
  return Remote.list(this);
};

/**
 * Get the status of a repo to it's working directory
 *
 * @async
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
 * @async
 * @param {obj} opts
 * @return {Array<StatusFile>}
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
 * Get the names of the submodules in the repository.
 *
 * @async
 * @return {Array<String>}
 */
Repository.prototype.getSubmoduleNames = function() {
  var names = [];
  var submoduleCallback = function(submodule, name, payload) {
    names.push(name);
  };

  return Submodule.foreach(this, submoduleCallback).then(function() {
    return names;
  });
};

/**
 * Retrieve the tag represented by the oid.
 *
 * @async
 * @param {String|Oid} String sha or Oid
 * @return {Tag}
 */
Repository.prototype.getTag = function(oid) {
  var repository = this;

  return Tag.lookup(repository, oid).then(function(reference) {
    reference.repo = repository;
    return reference;
  });
};

/**
 * Retrieve the tag represented by the tag name.
 *
 * @async
 * @param {String} Short or full tag name
 * @return {Tag}
 */
Repository.prototype.getTagByName = function(name) {
  var repo = this;

  name = ~name.indexOf("refs/tags/") ? name : "refs/tags/" + name;

  return Reference.nameToId(repo, name)
    .then(function(oid) {
      return Tag.lookup(repo, oid);
    }).then(function(reference) {
      reference.repo = repo;
      return reference;
    });
};

/**
 * Retrieve the tree represented by the oid.
 *
 * @async
 * @param {String|Oid} String sha or Oid
 * @return {Tree}
 */
Repository.prototype.getTree = function(oid) {
  var repository = this;

  return Tree.lookup(repository, oid).then(function(tree) {
    tree.repo = repository;
    return tree;
  });
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
 * Returns true if the repository is in the default NONE state.
 * @return {Boolean}
 */
Repository.prototype.isDefaultState = function() {
  return this.state() === NodeGit.Repository.STATE.NONE;
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

/**
 * Rebases a branch onto another branch
 *
 * @async
 * @param {String}     branch
 * @param {String}     upstream
 * @param {String}     onto
 * @param {Signature}  signature     Identity of the one performing the rebase
 * @param {Function}   beforeNextFn  Callback to be called before each step
 *                                   of the rebase.  If the callback returns a
 *                                   promise, the rebase will resume when the
 *                                   promise resolves.  The rebase object is
 *                                   is passed to the callback.
 * @param {Function}   beforeFinishFn Callback called before the invocation
 *                                    of finish(). If the callback returns a
 *                                    promise, finish() will be called when the
 *                                    promise resolves. This callback will be
 *                                    provided a detailed overview of the rebase
 * @param {RebaseOptions} rebaseOptions Options to initialize the rebase object
 *                                      with
 * @return {Oid|Index}  A commit id for a succesful merge or an index for a
 *                      rebase with conflicts
 */
Repository.prototype.rebaseBranches = function(
  branch,
  upstream,
  onto,
  signature,
  beforeNextFn,
  beforeFinishFn,
  rebaseOptions
)
{
  const repo = this;
  let branchCommit;
  let upstreamCommit;
  let ontoCommit;
  let mergeOptions = (rebaseOptions || {}).mergeOptions;

  let promiseChain = Promise.resolve();

  if (!signature) {
    promiseChain = promiseChain
      .then(() => repo.defaultSignature())
      .then((signatureResult) => {
        signature = signatureResult;
      });
  }

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
      return repo.mergeBranches(branch, upstream, null, null, mergeOptions)
        .then(function() {
          // checkout 'branch' to match the behavior of rebase
          return repo.checkoutBranch(branch);
        });
    } else if (oid.toString() === upstreamCommit.id().toString()) {
      // 'branch' is already on top of 'upstream'
      // checkout 'branch' to match the behavior of rebase
      return repo.checkoutBranch(branch);
    }

    return NodeGit.Rebase.init(
      repo,
      branchCommit,
      upstreamCommit,
      ontoCommit,
      rebaseOptions
    )
      .then(function(rebase) {
        return performRebase(
          repo,
          rebase,
          signature,
          beforeNextFn,
          beforeFinishFn
        );
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
 * Grabs a fresh copy of the index from the repository. Invalidates
 * all previously grabbed indexes
 *
 * @async
 * @return {Index}
 */
Repository.prototype.refreshIndex = function() {
  var repo = this;

  repo.setIndex(); // clear the index

  return repo.index();
};

/**
 * Merge a branch onto another branch
 *
 * @async
 * @param {String|Reference}        to
 * @param {String|Reference}        from
 * @param {Signature}         signature
 * @param {Merge.PREFERENCE}  mergePreference
 * @param {MergeOptions}      mergeOptions
 * @param {MergeBranchOptions} mergeBranchOptions
 * @return {Oid|Index}  A commit id for a succesful merge or an index for a
 *                      merge with conflicts
 */
Repository.prototype.mergeBranches = function(
  to,
  from,
  signature,
  mergePreference,
  mergeOptions,
  mergeBranchOptions
) {
  const repo = this;
  let fromBranch;
  let toBranch;
  // Support old parameter `processMergeMessageCallback`
  const isOldOptionParameter = typeof mergeBranchOptions === "function";
  if (isOldOptionParameter) {
    console.error("DeprecationWarning: Repository#mergeBranches parameter " +
                  "processMergeMessageCallback, use MergeBranchOptions");
  }
  const processMergeMessageCallback = mergeBranchOptions &&
    (isOldOptionParameter ?
      mergeBranchOptions :
      mergeBranchOptions.processMergeMessageCallback) ||
    function (message) { return message; };
  const signingCallback = mergeBranchOptions && mergeBranchOptions.signingCb;

  mergePreference = mergePreference || NodeGit.Merge.PREFERENCE.NONE;

  let promiseChain = Promise.resolve();

  if (!signature) {
    promiseChain = promiseChain
      .then(() => repo.defaultSignature())
      .then((signatureResult) => {
        signature = signatureResult;
      });
  }

  return promiseChain.then(() => Promise.all([
    repo.getBranch(to),
    repo.getBranch(from)
  ]))
  .then((objects) => {
    toBranch = objects[0];
    fromBranch = objects[1];

    return Promise.all([
      repo.getBranchCommit(toBranch),
      repo.getBranchCommit(fromBranch)
    ]);
  })
  .then((branchCommits) => {
    var toCommitOid = branchCommits[0].toString();
    var fromCommitOid = branchCommits[1].toString();

    return NodeGit.Merge.base(repo, toCommitOid, fromCommitOid)
    .then((baseCommit) => {
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
        .then((tree) => {
          if (toBranch.isHead()) {
            // Checkout the tree if we're on the branch
            var opts = {
              checkoutStrategy: NodeGit.Checkout.STRATEGY.SAFE |
                NodeGit.Checkout.STRATEGY.RECREATE_MISSING
            };
            return NodeGit.Checkout.tree(repo, tree, opts);
          }
        })
        .then(() => {
          return toBranch.setTarget(
            fromCommitOid,
            message)
          .then(() => {
            return fromCommitOid;
          });
        });
      }
      else if (mergePreference !== NodeGit.Merge.PREFERENCE.FASTFORWARD_ONLY) {
        var updateHead;
        // We have to merge. Lets do it!
        return NodeGit.Reference.lookup(repo, "HEAD")
        .then((headRef) => {
          return headRef.resolve();
        })
        .then((headRef) => {
          updateHead = !!headRef && (headRef.name() === toBranch.name());
          return NodeGit.Merge.commits(
            repo,
            toCommitOid,
            fromCommitOid,
            mergeOptions
          );
        })
        .then((index) => {
          // if we have conflicts then throw the index
          if (index.hasConflicts()) {
            throw index;
          }

          // No conflicts so just go ahead with the merge
          return index.writeTreeTo(repo);
        })
        .then((oid) => {
          var mergeDecorator;
          if (fromBranch.isTag()) {
            mergeDecorator = "tag";
          } else if (fromBranch.isRemote()) {
            mergeDecorator = "remote-tracking branch";
          } else {
            mergeDecorator = "branch";
          }

          var message =
            "Merge " +
            mergeDecorator +
            " '" +
            fromBranch.shorthand() +
            "'";

          // https://github.com/git/git/blob/master/builtin/fmt-merge-msg.c#L456-L459
          if (toBranch.shorthand() !== "master") {
            message += " into " + toBranch.shorthand();
          }

          return Promise.all([oid, processMergeMessageCallback(message)]);
        })
        .then(([oid, message]) => {
          if (signingCallback) {
            return repo.createCommitWithSignature(
              toBranch.name(),
              signature,
              signature,
              message,
              oid,
              [toCommitOid, fromCommitOid],
              signingCallback
            );
          }
          return repo.createCommit(
            toBranch.name(),
            signature,
            signature,
            message,
            oid,
            [toCommitOid, fromCommitOid]
          );
        })
        .then((commit) => {
          // we've updated the checked out branch, so make sure we update
          // head so that our index isn't messed up
          if (updateHead) {
            return repo.getBranch(to)
            .then((branch) => {
              return repo.getBranchCommit(branch);
            })
            .then((branchCommit) => {
              return branchCommit.getTree();
            })
            .then((tree) => {
              var opts = {
                checkoutStrategy: NodeGit.Checkout.STRATEGY.SAFE |
                  NodeGit.Checkout.STRATEGY.RECREATE_MISSING
              };
              return NodeGit.Checkout.tree(repo, tree, opts);
            })
            .then(() => {
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
 * @async
 * @param {MergeheadForeachCb} callback The callback function to be called on
 * each entry
 */
Repository.prototype.mergeheadForeach = function(callback) {
  return _mergeheadForeach.call(this, callback, null);
};

/**
 * Stages or unstages line selection of a specified file
 *
 * @async
 * @param {String|Array} filePath The relative path of this file in the repo
 * @param {Boolean} stageNew Set to stage new filemode. Unset to unstage.
 * @return {Number} 0 or an error code
 */
Repository.prototype.stageFilemode =
  function(filePath, stageNew, additionalDiffOptions) {
  var repo = this;
  var index;
  var diffOptions = additionalDiffOptions ? {
    flags: additionalDiffOptions
  } : undefined;
  var diffPromise = stageNew ?
    NodeGit.Diff.indexToWorkdir(repo, index, {
        flags:
          NodeGit.Diff.OPTION.SHOW_UNTRACKED_CONTENT |
          NodeGit.Diff.OPTION.RECURSE_UNTRACKED_DIRS |
          (additionalDiffOptions || 0)
      })
    :
    repo.getHeadCommit()
      .then(function getTreeFromCommit(commit) {
        return commit.getTree();
      })
      .then(function getDiffFromTree(tree) {
        return NodeGit.Diff.treeToIndex(repo, tree, index, diffOptions);
      });
  var filePaths = (filePath instanceof Array) ? filePath : [filePath];

  var indexLock = repo.path().replace(".git/", "") + ".git/index.lock";

  return fse.remove(indexLock)
    .then(function() {
      return repo.refreshIndex();
    })
    .then(function(indexResult) {
      index = indexResult;
    })
    .then(function() {
      return diffPromise;
    })
    .then(function(diff) {
      var origLength = filePaths.length;
      var fileFilterPromises = fp.map(function(p) {
        return NodeGit.Status.file(repo, p)
          .then(function(status) {
            return {
              path: p,
              filter: (
                (status & NodeGit.Status.STATUS.WT_MODIFIED) ||
                (status & NodeGit.Status.STATUS.INDEX_MODIFIED)
              )
            };
          });
      }, filePaths);

      return Promise.all(fileFilterPromises)
        .then(function(results) {
          filePaths = fp.flow([
            fp.filter(function(filterResult) {
              return filterResult.filter;
            }),
            fp.map(function(filterResult) {
              return filterResult.path;
            })
          ])(results);

          if (filePaths.length === 0 && origLength > 0) {
            return Promise.reject
                ("Selected staging is only available on modified files.");
          }
          return diff.patches();
        });
    })
    .then(function(patches) {
      var pathPatches = patches.filter(function(patch) {
        return ~filePaths.indexOf(patch.newFile().path());
      });
      if (pathPatches.length === 0) {
        return Promise.reject("No differences found for this file.");
      }

      return pathPatches
        .reduce(function(lastIndexAddPromise, pathPatch) {
          var entry = index.getByPath(pathPatch.newFile().path(), 0);

          entry.mode = stageNew ?
            pathPatch.newFile().mode() : pathPatch.oldFile().mode();

          return lastIndexAddPromise
            .then(function() {
              return index.add(entry);
            });
        }, Promise.resolve());
    })
    .then(function() {
      return index.write();
    });
};

/**
 * Stages or unstages line selection of a specified file
 *
 * @async
 * @param {String} filePath The relative path of this file in the repo
 * @param {Array} selectedLines The array of DiffLine objects
 *                            selected for staging or unstaging
 * @param {Boolean} isStaged Are the selected lines currently staged
 * @return {Number} 0 or an error code
 */
Repository.prototype.stageLines =
  function(filePath, selectedLines, isSelectionStaged, additionalDiffOptions) {

  var repo = this;
  var index;
  var originalBlob;

  // The following chain checks if there is a patch with no hunks left for the
  // file, and no filemode changes were done on the file. It is then safe to
  // stage the entire file so the file doesn't show as having unstaged changes
  // in `git status`. Also, check if there are no type changes.
  var lastHunkStagedPromise = function lastHunkStagedPromise(result) {
    return NodeGit.Diff.indexToWorkdir(repo, index, {
      flags:
        NodeGit.Diff.OPTION.SHOW_UNTRACKED_CONTENT |
        NodeGit.Diff.OPTION.RECURSE_UNTRACKED_DIRS |
        (additionalDiffOptions || 0)
    })
    .then(function(diff) {
      return diff.patches();
    })
    .then(function(patches) {
      var pathPatch = patches.filter(function(patch) {
        return patch.newFile().path() === filePath;
      });
      var emptyPatch = false;
      if (pathPatch.length > 0) {
        // No hunks, unchanged file mode, and no type changes.
        emptyPatch = pathPatch[0].size() === 0 &&
          pathPatch[0].oldFile().mode() === pathPatch[0].newFile().mode() &&
          !pathPatch[0].isTypeChange();
      }
      if (emptyPatch) {
        return index.addByPath(filePath)
          .then(function() {
            return index.write();
          });
      }

      return result;
    });
  };

  return repo.refreshIndex()
    .then(function(indexResult) {
      index = indexResult;
      var pathOid = index.getByPath(filePath).id;

      return repo.getBlob(pathOid);
    })
    .then(function(blob) {
      originalBlob = blob;

      return getPathHunks(
        repo,
        index,
        filePath,
        isSelectionStaged,
        additionalDiffOptions
      );
    })
    .then(function(hunks) {
      return applySelectedLinesToTarget(
        originalBlob, selectedLines, hunks, isSelectionStaged
      );
    })
    .then(function(newContent) {
      var newContentBuffer = Buffer.from(newContent);

      return repo.createBlobFromBuffer(newContentBuffer);
    })
    .then(newOid => repo.getBlob(newOid))
    .then(function(newBlob) {
      var entry = index.getByPath(filePath, 0);
      entry.id = newBlob.id();
      entry.path = filePath;
      entry.fileSize = newBlob.content().length;

      return index.add(entry);
    })
    .then(function() {
      return index.write();
    })
    .then(function(result) {
      if (isSelectionStaged) {
        return result;
      }

      return lastHunkStagedPromise(result);
    });
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
