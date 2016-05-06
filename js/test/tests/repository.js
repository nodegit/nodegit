var assert = require("assert");
var path = require("path");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));
var local = path.join.bind(path, __dirname);
var IndexUtils = require("../utils/index_setup");
var RepoUtils = require("../utils/repository_setup");

describe("Repository", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Index = NodeGit.Index;
  var Signature = NodeGit.Signature;

  var reposPath = local("../repos/workdir");
  var newRepoPath = local("../repos/newrepo");
  var emptyRepoPath = local("../repos/empty");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      })
      .then(function() {
        return Repository.open(emptyRepoPath);
      })
      .then(function(emptyRepo) {
        test.emptyRepo = emptyRepo;
      });
  });

  it("cannot instantiate a repository", function() {
    assert.throws(
      function() { new Repository(); },
      undefined,
      "hello"
    );
  });

  it("can open a valid repository", function() {
    assert.ok(this.repository instanceof Repository);
  });

  it("cannot open an invalid repository", function() {
    return Repository.open("repos/nonrepo")
      .then(null, function(err) {
        assert.ok(err instanceof Error);
      });
  });

  it("does not try to open paths that don't exist", function() {
    var missingPath = "/surely/this/directory/does/not/exist/on/this/machine";

    return Repository.open(missingPath)
      .then(null, function(err) {
        assert.ok(err instanceof Error);
      });
  });

  it("can initialize a repository into a folder", function() {
    return Repository.init(newRepoPath, 1)
      .then(function(path, isBare) {
        return Repository.open(newRepoPath);
      });
  });

  it("can utilize repository init options", function() {
    return fse.remove(newRepoPath)
      .then(function() {
        return Repository.initExt(newRepoPath, {
          flags: Repository.INIT_FLAG.MKPATH
        });
      });
  });

  it("can be cleaned", function() {
    this.repository.cleanup();

    // try getting a commit after cleanup (to test that the repo is usable)
    return this.repository.getHeadCommit()
      .then(function(commit) {
        assert.equal(
          commit.toString(),
          "32789a79e71fbc9e04d3eff7425e1771eb595150"
        );
      });
  });

  it("can read the index", function() {
    return this.repository.index()
      .then(function(index) {
        assert.ok(index instanceof Index);
      });
  });

  it("can list remotes", function() {
    return this.repository.getRemotes()
      .then(function(remotes) {
        assert.equal(remotes.length, 1);
        assert.equal(remotes[0], "origin");
      });
  });

  it("can get the current branch", function() {
    return this.repository.getCurrentBranch()
      .then(function(branch) {
        assert.equal(branch.shorthand(), "master");
      });
  });

  it("can get the default signature", function() {
    var sig = this.repository.defaultSignature();

    assert(sig instanceof Signature);
  });

  it("gets statuses with StatusFile", function() {
    var fileName = "my-new-file-that-shouldnt-exist.file";
    var fileContent = "new file from repository test";
    var repo = this.repository;
    var filePath = path.join(repo.workdir(), fileName);

    return fse.writeFile(filePath, fileContent)
      .then(function() {
        return repo.getStatus().then(function(statuses) {
          assert.equal(statuses.length, 1);
          assert.equal(statuses[0].path(), fileName);
          assert.ok(statuses[0].isNew());
        });
      })
      .then(function() {
        return fse.remove(filePath);
      })
      .catch(function (e) {
        return fse.remove(filePath)
          .then(function() {
            return Promise.reject(e);
          });
      });
  });

  it("gets extended statuses", function() {
    var fileName = "my-new-file-that-shouldnt-exist.file";
    var fileContent = "new file from repository test";
    var repo = this.repository;
    var filePath = path.join(repo.workdir(), fileName);

    return fse.writeFile(filePath, fileContent)
      .then(function() {
        return repo.getStatusExt();
      })
      .then(function(statuses) {
        assert.equal(statuses.length, 1);
        assert.equal(statuses[0].path(), fileName);
        assert.equal(statuses[0].indexToWorkdir().newFile().path(), fileName);
        assert.ok(statuses[0].isNew());
      })
      .then(function() {
        return fse.remove(filePath);
      })
      .catch(function (e) {
        return fse.remove(filePath)
          .then(function() {
            return Promise.reject(e);
          });
      });
  });

  it("gets fetch-heads", function() {
    var repo = this.repository;
    var foundMaster;

    return repo.fetch("origin", {
      credentials: function(url, userName) {
        return NodeGit.Cred.sshKeyFromAgent(userName);
      },
      certificateCheck: function() {
        return 1;
      }
    })
    .then(function() {
      return repo.fetchheadForeach(function(refname, remoteUrl, oid, isMerge) {
        if (refname == "refs/heads/master") {
          foundMaster = true;
          assert.equal(refname, "refs/heads/master");
          assert.equal(remoteUrl, "https://github.com/nodegit/test");
          assert.equal(
            oid.toString(),
            "32789a79e71fbc9e04d3eff7425e1771eb595150");
          assert.equal(isMerge, 1);
        }
      });
    })
    .then(function() {
      if (!foundMaster) {
        throw new Error("Couldn't find master in iteration of fetch heads");
      }
    });
  });

  it("can discover if a path is part of a repository", function() {
    var testPath = path.join(reposPath, "lib", "util", "normalize_oid.js");
    var expectedPath = path.join(reposPath, ".git");
    return NodeGit.Repository.discover(testPath, 0, "")
      .then(function(foundPath) {
        assert.equal(expectedPath, foundPath);
      });
  });

  it("can create a repo using initExt", function() {
    var initFlags = NodeGit.Repository.INIT_FLAG.NO_REINIT |
      NodeGit.Repository.INIT_FLAG.MKPATH |
      NodeGit.Repository.INIT_FLAG.MKDIR;
    return fse.remove(newRepoPath)
      .then(function() {
        return NodeGit.Repository.initExt(newRepoPath, { flags: initFlags });
      })
      .then(function() {
        return NodeGit.Repository.open(newRepoPath);
      });
  });

  it("will throw when a repo cannot be initialized using initExt", function() {
    var initFlags = NodeGit.Repository.INIT_FLAG.NO_REINIT |
      NodeGit.Repository.INIT_FLAG.MKPATH |
      NodeGit.Repository.INIT_FLAG.MKDIR;

    var nonsensePath = "gibberish";

    return NodeGit.Repository.initExt(nonsensePath, { flags: initFlags })
      .then(function() {
        assert.fail("Should have thrown an error.");
      })
      .catch(function(error) {
        assert(error, "Should have thrown an error.");
      });
  });

  it("can get the head commit", function() {
    return this.repository.getHeadCommit()
      .then(function(commit) {
        assert.equal(
          commit.toString(),
          "32789a79e71fbc9e04d3eff7425e1771eb595150"
        );
      });
  });

  it("returns null if there is no head commit", function() {
    return this.emptyRepo.getHeadCommit()
      .then(function(commit) {
        assert(!commit);
      });
  });

  it("can commit on head on a empty repo with createCommitOnHead", function() {
    var fileName = "my-new-file-that-shouldnt-exist.file";
    var fileContent = "new file from repository test";
    var repo = this.emptyRepo;
    var filePath = path.join(repo.workdir(), fileName);
    var authSig = repo.defaultSignature();
    var commitSig = repo.defaultSignature();
    var commitMsg = "Doug this has been commited";

    return fse.writeFile(filePath, fileContent)
      .then(function() {
        return repo.createCommitOnHead(
          [fileName],
          authSig,
          commitSig,
          commitMsg
        );
      })
      .then(function(oidResult) {
        return repo.getHeadCommit()
          .then(function(commit) {
            assert.equal(
              commit.toString(),
              oidResult.toString()
            );
          });
      });
  });

  it("can get all merge heads in a repo with mergeheadForeach", function() {
    var repo;
    var repoPath = local("../repos/merge-head");
    var ourBranchName = "ours";
    var theirBranchName = "theirs";
    var theirBranch;
    var fileName = "testFile.txt";
    var numMergeHeads = 0;
    var assertBranchTargetIs = function (theirBranch, mergeHead) {
      assert.equal(theirBranch.target(), mergeHead.toString());
      numMergeHeads++;
    };

    return RepoUtils.createRepository(repoPath)
      .then(function(_repo) {
        repo = _repo;
        return IndexUtils.createConflict(
          repo,
          ourBranchName,
          theirBranchName,
          fileName
        );
      })
      .then(function() {
        return repo.getBranch(theirBranchName);
      })
      .then(function(_theirBranch) {
        // Write the MERGE_HEAD file manually since createConflict does not
        theirBranch = _theirBranch;
        return fse.writeFile(
          path.join(repoPath, ".git", "MERGE_HEAD"),
          theirBranch.target().toString() + "\n"
        );
      })
      .then(function() {
        return repo.mergeheadForeach(
          assertBranchTargetIs.bind(this, theirBranch)
        );
      })
      .then(function() {
        assert.equal(numMergeHeads, 1);
      });
  });
});
