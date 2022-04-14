var assert = require("assert");
var path = require("path");
var fse = require("fs-extra");
var local = path.join.bind(path, __dirname);
var IndexUtils = require("../utils/index_setup");
var RepoUtils = require("../utils/repository_setup");

describe("Repository", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Index = NodeGit.Index;
  var Signature = NodeGit.Signature;

  var constReposPath = local("../repos/constworkdir");
  var reposPath = local("../repos/workdir");
  var newRepoPath = local("../repos/newrepo");
  var emptyRepoPath = local("../repos/empty");

  beforeEach(function() {
    var test = this;

    return Repository.open(constReposPath)
      .then(function(constRepository) {
        test.constRepository = constRepository;
      })
      .then(function() {
        return Repository.open(reposPath);
      })
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
    return this.repository.getRemoteNames()
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

  it("can get a reference commit", function() {
    return this.repository.getReferenceCommit("master")
      .then(function(commit) {
        assert.equal(
          "32789a79e71fbc9e04d3eff7425e1771eb595150",
          commit.toString()
        );
      });
  });

  it("can get the default signature", function() {
    this.repository.defaultSignature()
      .then((sig) => {
        assert(sig instanceof Signature);
      });
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
        return NodeGit.Credential.sshKeyFromAgent(userName);
      },
      certificateCheck: () => 0
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

  function discover(ceiling) {
    var testPath = path.join(reposPath, "lib", "util", "normalize_oid.js");
    var expectedPath = path.join(reposPath, ".git");
    return NodeGit.Repository.discover(testPath, 0, ceiling)
      .then(function(foundPath) {
        assert.equal(expectedPath, foundPath);
      });
  }

  it("can discover if a path is part of a repository, null ceiling",
      function() {
    return discover(null);
  });

  it("can discover if a path is part of a repository, empty ceiling",
      function() {
    return discover("");
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
    const fileName = "my-new-file-that-shouldnt-exist.file";
    const fileContent = "new file from repository test";
    const repo = this.emptyRepo;
    const filePath = path.join(repo.workdir(), fileName);
    const commitMsg = "Doug this has been commited";
    let authSig;
    let commitSig;

    return repo.defaultSignature()
      .then((sig) => {
        authSig = sig;
        commitSig = sig;
        return fse.writeFile(filePath, fileContent);
      })
      .then(() => {
        return repo.createCommitOnHead(
          [fileName],
          authSig,
          commitSig,
          commitMsg
        );
      })
      .then((oidResult) => {
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

  it("can obtain statistics from a valid constant repository", function() {
    return this.constRepository.statistics()
    .then(function(analysisReport) {

      assert.equal(analysisReport.repositorySize.commits.count, 993);
      assert.equal(analysisReport.repositorySize.commits.size, 265772);
      assert.equal(analysisReport.repositorySize.trees.count, 2418);
      assert.equal(analysisReport.repositorySize.trees.size, 1189264);
      assert.equal(analysisReport.repositorySize.trees.entries, 32590);
      assert.equal(analysisReport.repositorySize.blobs.count, 4150);
      assert.equal(analysisReport.repositorySize.blobs.size, 48489634);
      assert.equal(analysisReport.repositorySize.annotatedTags.count, 1);
      assert.equal(analysisReport.repositorySize.references.count, 9);

      assert.equal(analysisReport.biggestObjects.commits.maxSize, 956);
      assert.equal(analysisReport.biggestObjects.commits.maxParents, 2);
      assert.equal(analysisReport.biggestObjects.trees.maxEntries, 93);
      assert.equal(analysisReport.biggestObjects.blobs.maxSize, 1077756);

      assert.equal(analysisReport.historyStructure.maxDepth, 931);
      assert.equal(analysisReport.historyStructure.maxTagDepth, 1);

      assert.equal(analysisReport.biggestCheckouts.numDirectories, 128);
      assert.equal(analysisReport.biggestCheckouts.maxPathDepth, 10);
      assert.equal(analysisReport.biggestCheckouts.maxPathLength, 277);
      assert.equal(analysisReport.biggestCheckouts.numFiles, 514);
      assert.equal(analysisReport.biggestCheckouts.totalFileSize, 5160886);
      assert.equal(analysisReport.biggestCheckouts.numSymlinks, 2);
      assert.equal(analysisReport.biggestCheckouts.numSubmodules, 4);

      // console.log(JSON.stringify(analysisReport,null,2));
    });
  });
});
