var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var fse = require("fs-extra");

var garbageCollect = require("../utils/garbage_collect.js");

const isNode8 = process.versions.node.split(".")[0] === "8";

describe("Rebase", function() {
  var NodeGit = require("../../");
  var Checkout = NodeGit.Checkout;
  var Merge = NodeGit.Merge;
  var RepoUtils = require("../utils/repository_setup");

  var repoPath = local("../repos/rebase");
  var ourBranchName = "ours";
  var theirBranchName = "theirs";

  var removeFileFromIndex = function(repository, fileName) {
    return repository.refreshIndex()
      .then(function(index) {
        return index.removeByPath(fileName)
          .then(function() {
            return index.write();
          })
          .then(function() {
            return index.writeTree();
          });
      });
  };

  beforeEach(function() {
    var test = this;
    return RepoUtils.createRepository(repoPath)
      .then(function(repo) {
        test.repository = repo;
      });
  });

  after(function() {
    return fse.remove(repoPath);
  });

  it("can cleanly fast-forward via rebase", function() {
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var theirBranch;

    return fse.writeFile(path.join(repository.workdir(), ourFileName),
        ourFileContent)
      // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return RepoUtils.addFileToIndex(repository, ourFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "11ead82b1135b8e240fb5d61e703312fb9cc3d6a");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "we made a commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "91a183f87842ebb7a9b08dad8bc2473985796844");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "76631cb5a290dafe2959152626bb90f2a6d8ec94");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "0e9231d489b3f4303635fc4b0397830da095e7e7");
      })
      .then(function() {
        // unstage changes so that we can begin a rebase
        return removeFileFromIndex(repository, theirFileName);
      })
      .then(function() {
        return Promise.all([
          repository.getReference(ourBranchName),
          repository.getReference(theirBranchName)
        ]);
      })
      .then(function(refs) {
        assert.equal(refs.length, 2);

        return Promise.all([
          NodeGit.AnnotatedCommit.fromRef(repository, refs[0]),
          NodeGit.AnnotatedCommit.fromRef(repository, refs[1])
        ]);
      })
      .then(function(annotatedCommits) {
        assert.equal(annotatedCommits.length, 2);

        var ourAnnotatedCommit = annotatedCommits[0];
        var theirAnnotatedCommit = annotatedCommits[1];

        assert.equal(ourAnnotatedCommit.id().toString(),
          "91a183f87842ebb7a9b08dad8bc2473985796844");
        assert.equal(theirAnnotatedCommit.id().toString(),
          "0e9231d489b3f4303635fc4b0397830da095e7e7");

        return NodeGit.Rebase.init(repository, ourAnnotatedCommit,
          theirAnnotatedCommit, theirAnnotatedCommit);
      })
      .then(function(rebase) {
        assert.equal(rebase.operationEntrycount(), 0);

        return rebase.finish(ourSignature);
      })
      .then(function() {
        return repository.getBranchCommit(ourBranchName);
      })
      .then(function(commit) {
        assert.equal(commit.id().toString(),
          "0e9231d489b3f4303635fc4b0397830da095e7e7");
      });
  });

  it("can cleanly rebase a branch in-memory", function() {
    var baseFileName = "baseNewFile.txt";
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var baseFileContent = "How do you feel about Toll Roads?";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var ourBranch;
    var theirBranch;
    var rebase;

    return fse.writeFile(path.join(repository.workdir(), baseFileName),
      baseFileContent)
      // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return RepoUtils.addFileToIndex(repository, baseFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "b5cdc109d437c4541a13fb7509116b5f03d5039a");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "initial commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "be03abdf0353d05924c53bebeb0e5bb129cda44a");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

        return removeFileFromIndex(repository, theirFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), theirFileName));
      })
      .then(function() {
        return fse.writeFile(path.join(repository.workdir(), ourFileName),
          ourFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, ourFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "77867fc0bfeb3f80ab18a78c8d53aa3a06207047");

          return repository.createCommit(ourBranch.name(), ourSignature,
            ourSignature, "we made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

        return removeFileFromIndex(repository, ourFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), ourFileName));
      })
      .then(function() {
        return repository.checkoutBranch(ourBranchName);
      })
      .then(function() {
        return Promise.all([
          repository.getReference(ourBranchName),
          repository.getReference(theirBranchName)
        ]);
      })
      .then(function(refs) {
        assert.equal(refs.length, 2);

        return Promise.all([
          NodeGit.AnnotatedCommit.fromRef(repository, refs[0]),
          NodeGit.AnnotatedCommit.fromRef(repository, refs[1])
        ]);
      })
      .then(function(annotatedCommits) {
        assert.equal(annotatedCommits.length, 2);

        var ourAnnotatedCommit = annotatedCommits[0];
        var theirAnnotatedCommit = annotatedCommits[1];

        assert.equal(ourAnnotatedCommit.id().toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");
        assert.equal(theirAnnotatedCommit.id().toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

        var rebaseOptions = new NodeGit.RebaseOptions();
        rebaseOptions.inmemory = 1;

        return NodeGit.Rebase.init(repository, ourAnnotatedCommit,
          theirAnnotatedCommit, undefined, rebaseOptions);
      })
      .then(function(newRebase) {
        rebase = newRebase;

        // there should only be 1 rebase operation to perform
        assert.equal(rebase.operationEntrycount(), 1);

        return rebase.next();
      })
      .then(function(rebaseOperation) {
        assert.equal(rebaseOperation.type(),
          NodeGit.RebaseOperation.REBASE_OPERATION.PICK);
        assert.equal(rebaseOperation.id().toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

        return rebase.commit(null, ourSignature);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "b937100ee0ea17ef20525306763505a7fe2be29e");

        // git_rebase_operation_current returns the index of the rebase
        // operation that was last applied, so after the first operation, it
        // should be 0.
        assert.equal(rebase.operationCurrent(), 0);

        return rebase.finish(ourSignature, {});
      })
      .then(function(result) {
        assert.equal(result, 0);

        return repository.getBranchCommit(ourBranchName);
      })
      .then(function(commit) {
        // verify that the "ours" branch has NOT moved.
        // In-memory rebase does not touch refs.
        assert.equal(commit.id().toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

        // Lookup the new commit
        return NodeGit.Commit.lookup(repository,
          "b937100ee0ea17ef20525306763505a7fe2be29e");
      })
      .then(function(commit) {
        // Lookup the parent of our new commit
        return commit.parent(0);
      })
      .then(function(commit) {
        // verify that we are on top of "their commit"
        assert.equal(commit.id().toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");
      });
  });

  it("can cleanly rebase a branch onto another branch", function() {
    var baseFileName = "baseNewFile.txt";
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var baseFileContent = "How do you feel about Toll Roads?";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = NodeGit.Signature.create
    ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
    ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var ourBranch;
    var theirBranch;
    var rebase;

    return fse.writeFile(path.join(repository.workdir(), baseFileName),
      baseFileContent)
    // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return RepoUtils.addFileToIndex(repository, baseFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "b5cdc109d437c4541a13fb7509116b5f03d5039a");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "initial commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "be03abdf0353d05924c53bebeb0e5bb129cda44a");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

        return removeFileFromIndex(repository, theirFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), theirFileName));
      })
      .then(function() {
        return fse.writeFile(path.join(repository.workdir(), ourFileName),
          ourFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, ourFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "77867fc0bfeb3f80ab18a78c8d53aa3a06207047");

        return repository.createCommit(ourBranch.name(), ourSignature,
          ourSignature, "we made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

        return removeFileFromIndex(repository, ourFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), ourFileName));
      })
      .then(function() {
        return repository.checkoutBranch(ourBranchName);
      })
      .then(function() {
        return Promise.all([
          repository.getReference(ourBranchName),
          repository.getReference(theirBranchName)
        ]);
      })
      .then(function(refs) {
        assert.equal(refs.length, 2);

        return Promise.all([
          NodeGit.AnnotatedCommit.fromRef(repository, refs[0]),
          NodeGit.AnnotatedCommit.fromRef(repository, refs[1])
        ]);
      })
      .then(function(annotatedCommits) {
        assert.equal(annotatedCommits.length, 2);

        var ourAnnotatedCommit = annotatedCommits[0];
        var theirAnnotatedCommit = annotatedCommits[1];

        assert.equal(ourAnnotatedCommit.id().toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");
        assert.equal(theirAnnotatedCommit.id().toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

        return NodeGit.Rebase.init(repository, ourAnnotatedCommit,
          theirAnnotatedCommit, null);
      })
      .then(function(newRebase) {
        rebase = newRebase;

        // there should only be 1 rebase operation to perform
        assert.equal(rebase.operationEntrycount(), 1);

        return rebase.next();
      })
      .then(function(rebaseOperation) {
        assert.equal(rebaseOperation.type(),
          NodeGit.RebaseOperation.REBASE_OPERATION.PICK);
        assert.equal(rebaseOperation.id().toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

        return rebase.commit(null, ourSignature);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "b937100ee0ea17ef20525306763505a7fe2be29e");

        // git_rebase_operation_current returns the index of the rebase
        // operation that was last applied, so after the first operation, it
        // should be 0.
        assert.equal(rebase.operationCurrent(), 0);

        return rebase.finish(ourSignature, {});
      })
      .then(function(result) {
        assert.equal(result, 0);

        return repository.getBranchCommit(ourBranchName);
      })
      .then(function(commit) {
        // verify that the "ours" branch has moved to the correct place
        assert.equal(commit.id().toString(),
          "b937100ee0ea17ef20525306763505a7fe2be29e");

        return commit.parent(0);
      })
      .then(function(commit) {
        // verify that we are on top of "their commit"
        assert.equal(commit.id().toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");
      });
  });

  it("can rebase 2 branches with conflicts on a single file", function() {
    var fileName = "everyonesFile.txt";

    var baseFileContent = "How do you feel about Toll Roads?\n";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!\n";
    var theirFileContent = "I'm skeptical about Toll Roads\n";

    var expectedConflictedFileContent =
      "How do you feel about Toll Roads?\n" +
      "<<<<<<< theirs\n" +
      "I'm skeptical about Toll Roads\n" +
      "=======\n" +
      "I like Toll Roads. I have an EZ-Pass!\n" +
      ">>>>>>> we made a commit\n";

    var conflictSolvedFileContent =
      "How do you feel about Toll Roads?\n" +
      "He's skeptical about Toll Roads,\n" +
      "but I like Toll Roads. I have an EZ-Pass!\n";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var ourBranch;
    var theirBranch;
    var rebase;

    return fse.writeFile(path.join(repository.workdir(), fileName),
      baseFileContent)
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "044704f62399fecbe22da6d7d47b14e52625630e");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "initial commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "80111c46ac73b857a3493b24c81df08639b5de99");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), fileName),
          baseFileContent + theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "b826e989aca7647bea64810f0a2a38acbbdd4c1a");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "b3c355bb606ec7da87174dfa1a0b0c0e3dc97bc0");

        return fse.writeFile(path.join(repository.workdir(), fileName),
          baseFileContent + ourFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "e7fe41bf7c0c28766887a63ffe2f03f624276fbe");

          return repository.createCommit(ourBranch.name(), ourSignature,
            ourSignature, "we made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "28cfeb17f66132edb3c4dacb7ff38e8dd48a1844");

        var opts = {
          checkoutStrategy: NodeGit.Checkout.STRATEGY.FORCE
        };

        return NodeGit.Checkout.head(repository, opts);
      })
      .then(function() {
        return Promise.all([
          repository.getReference(ourBranchName),
          repository.getReference(theirBranchName)
        ]);
      })
      .then(function(refs) {
        assert.equal(refs.length, 2);

        return Promise.all([
          NodeGit.AnnotatedCommit.fromRef(repository, refs[0]),
          NodeGit.AnnotatedCommit.fromRef(repository, refs[1])
        ]);
      })
      .then(function(annotatedCommits) {
        assert.equal(annotatedCommits.length, 2);

        var ourAnnotatedCommit = annotatedCommits[0];
        var theirAnnotatedCommit = annotatedCommits[1];

        assert.equal(ourAnnotatedCommit.id().toString(),
          "28cfeb17f66132edb3c4dacb7ff38e8dd48a1844");
        assert.equal(theirAnnotatedCommit.id().toString(),
          "b3c355bb606ec7da87174dfa1a0b0c0e3dc97bc0");

        return NodeGit.Rebase.init(repository, ourAnnotatedCommit,
          theirAnnotatedCommit, null);
      })
      .then(function(newRebase) {
        rebase = newRebase;

        // there should only be 1 rebase operation to perform
        assert.equal(rebase.operationEntrycount(), 1);

        return rebase.next();
      })
      .then(function(rebaseOperation) {
        assert.equal(rebaseOperation.type(),
          NodeGit.RebaseOperation.REBASE_OPERATION.PICK);
        assert.equal(rebaseOperation.id().toString(),
          "28cfeb17f66132edb3c4dacb7ff38e8dd48a1844");

        return repository.refreshIndex()
          .then(function(index) {
            assert.ok(index.hasConflicts());
          });
      })
      .then(function() {
        return fse.readFile(path.join(repository.workdir(), fileName), "utf8")
          .then(function(fileContent) {
            assert.equal(fileContent, expectedConflictedFileContent);

            return fse.writeFile(path.join(repository.workdir(), fileName),
              conflictSolvedFileContent);
          });
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        return repository.refreshIndex()
          .then(function(index) {
            assert.ok(!index.hasConflicts());

            return rebase.commit(null, ourSignature);
          });
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "ef6d0e95167435b3d58f51ab165948c72f6f94b6");

        return rebase.finish(ourSignature);
      })
      .then(function(result) {
        assert.equal(result, 0);

        return repository.getBranchCommit(ourBranchName);
      })
      .then(function(commit) {
        // verify that the "ours" branch has moved to the correct place
        assert.equal(commit.id().toString(),
          "ef6d0e95167435b3d58f51ab165948c72f6f94b6");

        return commit.parent(0);
      })
      .then(function(commit) {
        // verify that we are on top of "their commit"
        assert.equal(commit.id().toString(),
          "b3c355bb606ec7da87174dfa1a0b0c0e3dc97bc0");
      });
  });

  it("can abort an in-progress rebase", function() {
    var baseFileName = "baseNewFile.txt";
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var baseFileContent = "How do you feel about Toll Roads?";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var ourBranch;
    var theirBranch;
    var rebase;

    return fse.writeFile(path.join(repository.workdir(), baseFileName),
      baseFileContent)
      // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return RepoUtils.addFileToIndex(repository, baseFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "b5cdc109d437c4541a13fb7509116b5f03d5039a");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "initial commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "be03abdf0353d05924c53bebeb0e5bb129cda44a");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

        return removeFileFromIndex(repository, theirFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), theirFileName));
      })
      .then(function() {
        return fse.writeFile(path.join(repository.workdir(), ourFileName),
          ourFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, ourFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "77867fc0bfeb3f80ab18a78c8d53aa3a06207047");

          return repository.createCommit(ourBranch.name(), ourSignature,
            ourSignature, "we made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

        return removeFileFromIndex(repository, ourFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), ourFileName));
      })
      .then(function() {
        return repository.checkoutBranch(ourBranchName);
      })
      .then(function() {
        return Promise.all([
          repository.getReference(ourBranchName),
          repository.getReference(theirBranchName)
        ]);
      })
      .then(function(refs) {
        assert.equal(refs.length, 2);

        return Promise.all([
          NodeGit.AnnotatedCommit.fromRef(repository, refs[0]),
          NodeGit.AnnotatedCommit.fromRef(repository, refs[1])
        ]);
      })
      .then(function(annotatedCommits) {
        assert.equal(annotatedCommits.length, 2);

        var ourAnnotatedCommit = annotatedCommits[0];
        var theirAnnotatedCommit = annotatedCommits[1];

        assert.equal(ourAnnotatedCommit.id().toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");
        assert.equal(theirAnnotatedCommit.id().toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

        return NodeGit.Rebase.init(repository, ourAnnotatedCommit,
          theirAnnotatedCommit, null);
      })
      .then(function(newRebase) {
        rebase = newRebase;

        // there should only be 1 rebase operation to perform
        assert.equal(rebase.operationEntrycount(), 1);

        return rebase.next();
      })
      .then(function(rebaseOperation) {
        assert.equal(rebaseOperation.type(),
          NodeGit.RebaseOperation.REBASE_OPERATION.PICK);
        assert.equal(rebaseOperation.id().toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

        return rebase.commit(null, ourSignature);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "b937100ee0ea17ef20525306763505a7fe2be29e");

        return repository.getBranchCommit("HEAD")
          .then(function(commit) {
            // verify that HEAD is on the rebased commit
            assert.equal(commit.id().toString(), commitOid.toString());
          });
      })
      .then(function() {
        return rebase.abort();
      })
      .then(function() {
        return NodeGit.Rebase.open(repository)
          .then(function(existingRebase) {
            assert.fail(existingRebase, undefined,
              "There should not be a rebase in progress");
          })
          .catch(function(e) {
            assert.equal(e.message, "there is no rebase in progress");
          });
      })
      .then(function() {
        return Promise.all([
          repository.getBranchCommit("HEAD"),
          repository.getBranchCommit(ourBranchName)
        ]);
      })
      .then(function(commits) {
        assert.equal(commits.length, 2);

        // verify that 'HEAD' and 'ours' are back to their pre-rebase state
        assert.equal(commits[0].id().toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");
        assert.equal(commits[1].id().toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");
      });
  });

  it("can fast-forward via rebase using the convenience methods",
    function() {
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var theirBranch;

    return fse.writeFile(path.join(repository.workdir(), ourFileName),
        ourFileContent)
      // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return RepoUtils.addFileToIndex(repository, ourFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "11ead82b1135b8e240fb5d61e703312fb9cc3d6a");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "we made a commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "91a183f87842ebb7a9b08dad8bc2473985796844");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "76631cb5a290dafe2959152626bb90f2a6d8ec94");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "0e9231d489b3f4303635fc4b0397830da095e7e7");
      })
      .then(function() {
        // unstage changes so that we can begin a rebase
        return removeFileFromIndex(repository, theirFileName);
      })
      .then(function() {
        return Promise.all([
          repository.getReference(ourBranchName),
          repository.getReference(theirBranchName)
        ]);
      })
      .then(function(refs) {
        assert.equal(refs.length, 2);

        return Promise.all([
          NodeGit.AnnotatedCommit.fromRef(repository, refs[0]),
          NodeGit.AnnotatedCommit.fromRef(repository, refs[1])
        ]);
      })
      .then(function(annotatedCommits) {
        assert.equal(annotatedCommits.length, 2);

        var ourAnnotatedCommit = annotatedCommits[0];
        var theirAnnotatedCommit = annotatedCommits[1];

        assert.equal(ourAnnotatedCommit.id().toString(),
          "91a183f87842ebb7a9b08dad8bc2473985796844");
        assert.equal(theirAnnotatedCommit.id().toString(),
          "0e9231d489b3f4303635fc4b0397830da095e7e7");

        return fse.remove(path.join(repository.workdir(), theirFileName));
      })
      .then(function() {
        return repository.rebaseBranches(ourBranchName, theirBranchName, null,
          ourSignature);
      })
      .then(function(commit) {
        assert.equal(commit.id().toString(),
          "0e9231d489b3f4303635fc4b0397830da095e7e7");
      });
  });

  it(
    "can fast-forward a merge commit via rebase using the " +
    "convenience methods that has a beforeFinishFn",
    function() {
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";
    var theirOtherFileName = "antherNewFile.txt";

    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";
    var theirOtherFileContent = "This is some more content, guys!";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);
    var theirOtherSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456999, 60);
    var ourMergeSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456889, 60);

    var repository = this.repository;
    var ourCommit;
    var theirCommit;
    var theirBranch;
    var ourBranch;

    return fse.writeFile(path.join(repository.workdir(), ourFileName),
        ourFileContent)
      // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return RepoUtils.addFileToIndex(repository, ourFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "11ead82b1135b8e240fb5d61e703312fb9cc3d6a");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "we made a commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "91a183f87842ebb7a9b08dad8bc2473985796844");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "76631cb5a290dafe2959152626bb90f2a6d8ec94");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        theirCommit = commitOid;
        assert.equal(commitOid.toString(),
          "0e9231d489b3f4303635fc4b0397830da095e7e7");
      })
      .then(function() {
        return repository.checkoutBranch(
          ourBranch,
          { checkoutStrategy: Checkout.STRATEGY.FORCE }
        );
      })
      .then(function() {
        return repository.mergeBranches(
          ourBranchName,
          theirBranchName,
          ourMergeSignature,
          Merge.PREFERENCE.NO_FASTFORWARD
        );
      })
      .then(function() {
        return repository.getHeadCommit();
      })
      .then(function(headCommit) {
        assert.notEqual(ourCommit.id().toString(), headCommit.id().toString());
      })
      .then(function() {
        return repository.checkoutBranch(
          theirBranch,
          { checkoutStrategy: Checkout.STRATEGY.FORCE }
        );
      })
      .then(function() {
        return fse.writeFile(
          path.join(repository.workdir(), theirOtherFileName),
          theirOtherFileContent
        );
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, theirOtherFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "c242b53f2c9446544cf9bdac7e8ed6ce583226cb");

        return repository.createCommit(theirBranch.name(), theirOtherSignature,
        theirOtherSignature, "they made another commit", oid, [theirCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "8fa0ce25a2accf464b004ddeeb63add7b816b627");
      })
      .then(function() {
        return Promise.all([
          repository.getReference(ourBranchName),
          repository.getReference(theirBranchName)
        ]);
      })
      .then(function(refs) {
        assert.equal(refs.length, 2);

        return Promise.all([
          NodeGit.AnnotatedCommit.fromRef(repository, refs[0]),
          NodeGit.AnnotatedCommit.fromRef(repository, refs[1])
        ]);
      })
      .then(function(annotatedCommits) {
        assert.equal(annotatedCommits.length, 2);

        var ourAnnotatedCommit = annotatedCommits[0];
        var theirAnnotatedCommit = annotatedCommits[1];

        assert.equal(ourAnnotatedCommit.id().toString(),
          "0d1d322b59df68bac6eea6a2a189f974cb590368");
        assert.equal(theirAnnotatedCommit.id().toString(),
          "8fa0ce25a2accf464b004ddeeb63add7b816b627");

        return repository.rebaseBranches(
          ourBranchName,
          theirBranchName,
          null,
          ourSignature,
          null,
          function(rebaseData) {
            assert.equal(rebaseData.rewritten, null);
          }
        );
      })
      .then(function(commit) {
        assert.equal(commit.id().toString(),
          "8fa0ce25a2accf464b004ddeeb63add7b816b627");
      });
  });

  it("can rebase using the convenience method", function() {
    var baseFileName = "baseNewFile.txt";
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var baseFileContent = "How do you feel about Toll Roads?";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var ourBranch;
    var theirBranch;

    var nextCalls = 0;

    return fse.writeFile(path.join(repository.workdir(), baseFileName),
      baseFileContent)
      // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return RepoUtils.addFileToIndex(repository, baseFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "b5cdc109d437c4541a13fb7509116b5f03d5039a");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "initial commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "be03abdf0353d05924c53bebeb0e5bb129cda44a");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

        return removeFileFromIndex(repository, theirFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), theirFileName));
      })
      .then(function() {
        return fse.writeFile(path.join(repository.workdir(), ourFileName),
          ourFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, ourFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "77867fc0bfeb3f80ab18a78c8d53aa3a06207047");

          return repository.createCommit(ourBranch.name(), ourSignature,
            ourSignature, "we made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

        return removeFileFromIndex(repository, ourFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), ourFileName));
      })
      .then(function() {
        return repository.checkoutBranch(ourBranchName);
      })
      .then(function() {
        return repository.rebaseBranches(ourBranchName, theirBranchName,
          null, ourSignature, function(rebase) {
            assert.ok(rebase instanceof NodeGit.Rebase);

            nextCalls++;

            return Promise.resolve();
          });
      })
      .then(function(commit) {
        // verify that the beforeNextFn callback was called
        assert.equal(nextCalls, 2);

        // verify that the "ours" branch has moved to the correct place
        assert.equal(commit.id().toString(),
          "b937100ee0ea17ef20525306763505a7fe2be29e");

        return commit.parent(0);
      })
      .then(function(commit) {
        // verify that we are on top of "their commit"
        assert.equal(commit.id().toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");
      });
  });

  it("beforeFinishFn sync callback receives correct rebase data", function() {
    var baseFileName = "baseNewFile.txt";
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var baseFileContent = "How do you feel about Toll Roads?";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var ourBranch;
    var theirBranch;
    var ourBranchShaPreRebase;
    var ourBranchShaPostRebase = "b937100ee0ea17ef20525306763505a7fe2be29e";
    var theirBranchSha;

    var nextCalls = 0;

    var calledBeforeFinishFn = false;

    return fse.writeFile(path.join(repository.workdir(), baseFileName),
      baseFileContent)
      // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return RepoUtils.addFileToIndex(repository, baseFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "b5cdc109d437c4541a13fb7509116b5f03d5039a");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "initial commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "be03abdf0353d05924c53bebeb0e5bb129cda44a");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        theirBranchSha = commitOid.toString();
        assert.equal(theirBranchSha,
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

        return removeFileFromIndex(repository, theirFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), theirFileName));
      })
      .then(function() {
        return fse.writeFile(path.join(repository.workdir(), ourFileName),
          ourFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, ourFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "77867fc0bfeb3f80ab18a78c8d53aa3a06207047");

          return repository.createCommit(ourBranch.name(), ourSignature,
            ourSignature, "we made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        ourBranchShaPreRebase = commitOid.toString();
        assert.equal(ourBranchShaPreRebase,
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

        return removeFileFromIndex(repository, ourFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), ourFileName));
      })
      .then(function() {
        return repository.checkoutBranch(ourBranchName);
      })
      .then(function() {
        return repository.rebaseBranches(ourBranchName, theirBranchName,
          null, ourSignature, function(rebase) {
            assert.ok(rebase instanceof NodeGit.Rebase);

            nextCalls++;

            return Promise.resolve();
          }, function(rebaseMetadata) {
            calledBeforeFinishFn = true;

            assert.equal(rebaseMetadata.ontoName, theirBranchName);
            assert.equal(rebaseMetadata.ontoSha, theirBranchSha);
            assert.equal(rebaseMetadata.originalHeadName, ourBranchName);
            assert.equal(
              rebaseMetadata.originalHeadSha,
              ourBranchShaPreRebase
            );
            assert.deepEqual(
              rebaseMetadata.rewritten,
              [[ourBranchShaPreRebase, ourBranchShaPostRebase]]
            );
          });
      })
      .then(function(commit) {
        // verify that the beforeNextFn callback was called
        assert.equal(nextCalls, 2);

        // verify that the beforeFinishFn callback was called
        assert(calledBeforeFinishFn, "beforeFinishFn was not called");

        // verify that the "ours" branch has moved to the correct place
        assert.equal(commit.id().toString(), ourBranchShaPostRebase);

        return commit.parent(0);
      })
      .then(function(commit) {
        // verify that we are on top of "their commit"
        assert.equal(commit.id().toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");
      });
  });

  it("beforeFinishFn async callback receives correct rebase data", function() {
    var baseFileName = "baseNewFile.txt";
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var baseFileContent = "How do you feel about Toll Roads?";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var ourBranch;
    var theirBranch;
    var ourBranchShaPreRebase;
    var ourBranchShaPostRebase = "b937100ee0ea17ef20525306763505a7fe2be29e";
    var theirBranchSha;

    var nextCalls = 0;

    var calledBeforeFinishFn = false;

    return fse.writeFile(path.join(repository.workdir(), baseFileName),
      baseFileContent)
      // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return RepoUtils.addFileToIndex(repository, baseFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "b5cdc109d437c4541a13fb7509116b5f03d5039a");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "initial commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "be03abdf0353d05924c53bebeb0e5bb129cda44a");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        theirBranchSha = commitOid.toString();
        assert.equal(theirBranchSha,
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

        return removeFileFromIndex(repository, theirFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), theirFileName));
      })
      .then(function() {
        return fse.writeFile(path.join(repository.workdir(), ourFileName),
          ourFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, ourFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "77867fc0bfeb3f80ab18a78c8d53aa3a06207047");

          return repository.createCommit(ourBranch.name(), ourSignature,
            ourSignature, "we made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        ourBranchShaPreRebase = commitOid.toString();
        assert.equal(ourBranchShaPreRebase,
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

        return removeFileFromIndex(repository, ourFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), ourFileName));
      })
      .then(function() {
        return repository.checkoutBranch(ourBranchName);
      })
      .then(function() {
        return repository.rebaseBranches(ourBranchName, theirBranchName,
          null, ourSignature, function(rebase) {
            assert.ok(rebase instanceof NodeGit.Rebase);

            nextCalls++;

            return Promise.resolve();
          }, function(rebaseMetadata) {
            calledBeforeFinishFn = true;

            assert.equal(rebaseMetadata.ontoName, theirBranchName);
            assert.equal(rebaseMetadata.ontoSha, theirBranchSha);
            assert.equal(rebaseMetadata.originalHeadName, ourBranchName);
            assert.equal(
              rebaseMetadata.originalHeadSha,
              ourBranchShaPreRebase
            );
            assert.deepEqual(
              rebaseMetadata.rewritten,
              [[ourBranchShaPreRebase, ourBranchShaPostRebase]]
            );

            return Promise.resolve();
          });
      })
      .then(function(commit) {
        // verify that the beforeNextFn callback was called
        assert.equal(nextCalls, 2);

        // verify that the beforeFinishFn callback was called
        assert(calledBeforeFinishFn, "beforeFinishFn was not called");

        // verify that the "ours" branch has moved to the correct place
        assert.equal(commit.id().toString(), ourBranchShaPostRebase);

        return commit.parent(0);
      })
      .then(function(commit) {
        // verify that we are on top of "their commit"
        assert.equal(commit.id().toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");
      });
  });

  it("can rebase with conflicts using the convenience methods", function() {
    var fileName = "everyonesFile.txt";

    var baseFileContent = "How do you feel about Toll Roads?\n";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!\n";
    var theirFileContent = "I'm skeptical about Toll Roads\n";

    var expectedConflictedFileContent =
      "How do you feel about Toll Roads?\n" +
      "<<<<<<< theirs\n" +
      "I'm skeptical about Toll Roads\n" +
      "=======\n" +
      "I like Toll Roads. I have an EZ-Pass!\n" +
      ">>>>>>> we made a commit\n";

    var conflictSolvedFileContent =
      "How do you feel about Toll Roads?\n" +
      "He's skeptical about Toll Roads,\n" +
      "but I like Toll Roads. I have an EZ-Pass!\n";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var ourBranch;
    var theirBranch;
    var nextCalls=0;

    return fse.writeFile(path.join(repository.workdir(), fileName),
      baseFileContent)
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "044704f62399fecbe22da6d7d47b14e52625630e");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "initial commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "80111c46ac73b857a3493b24c81df08639b5de99");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), fileName),
          baseFileContent + theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "b826e989aca7647bea64810f0a2a38acbbdd4c1a");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "b3c355bb606ec7da87174dfa1a0b0c0e3dc97bc0");

        return fse.writeFile(path.join(repository.workdir(), fileName),
          baseFileContent + ourFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "e7fe41bf7c0c28766887a63ffe2f03f624276fbe");

          return repository.createCommit(ourBranch.name(), ourSignature,
            ourSignature, "we made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "28cfeb17f66132edb3c4dacb7ff38e8dd48a1844");

        var opts = {
          checkoutStrategy: NodeGit.Checkout.STRATEGY.FORCE
        };

        return NodeGit.Checkout.head(repository, opts);
      })
      .then(function() {
        return repository.rebaseBranches(ourBranchName, theirBranchName,
          null, ourSignature)
          .then(function(commit) {
            assert.fail(commit, undefined,
              "The index should have been thrown due to merge conflicts");
          })
          .catch(function(index) {
            assert.ok(index);
            assert.ok(index.hasConflicts());

            assert.ok(repository.isRebasing());
          });
      })
      .then(function() {
        return fse.readFile(path.join(repository.workdir(), fileName), "utf8")
          .then(function(fileContent) {
            assert.equal(fileContent, expectedConflictedFileContent);

            return fse.writeFile(path.join(repository.workdir(), fileName),
              conflictSolvedFileContent);
          });
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        return repository.refreshIndex()
          .then(function(index) {
            assert.ok(!index.hasConflicts());

            return repository.continueRebase(ourSignature, function(rebase) {
              assert.ok(rebase instanceof NodeGit.Rebase);

              nextCalls++;

              return Promise.resolve();
            });
          });
      })
      .then(function(commit) {
        // verify that the beforeNextFn callback was called
        assert.equal(nextCalls, 1);

        // verify that the "ours" branch has moved to the correct place
        assert.equal(commit.id().toString(),
          "ef6d0e95167435b3d58f51ab165948c72f6f94b6");

        assert.ok(!repository.isRebasing());
        assert.ok(repository.isDefaultState());

        return commit.parent(0);
      })
      .then(function(commit) {
        // verify that we are on top of "their commit"
        assert.equal(commit.id().toString(),
          "b3c355bb606ec7da87174dfa1a0b0c0e3dc97bc0");
      });
  });

  if (!isNode8) {
    it("can sign commits during the rebase", function() {
      var baseFileName = "baseNewFile.txt";
      var ourFileName = "ourNewFile.txt";
      var theirFileName = "theirNewFile.txt";

      var baseFileContent = "How do you feel about Toll Roads?";
      var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
      var theirFileContent = "I'm skeptical about Toll Roads";

      var ourSignature = NodeGit.Signature.create
      ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
      var theirSignature = NodeGit.Signature.create
      ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

      var repository = this.repository;
      var ourCommit;
      var ourBranch;
      var theirBranch;
      var rebase;

      return fse.writeFile(path.join(repository.workdir(), baseFileName),
      baseFileContent)
      // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return RepoUtils.addFileToIndex(repository, baseFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
        "b5cdc109d437c4541a13fb7509116b5f03d5039a");

        return repository.createCommit("HEAD", ourSignature,
        ourSignature, "initial commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
        "be03abdf0353d05924c53bebeb0e5bb129cda44a");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
          .then(function(branch) {
            ourBranch = branch;
            return repository.createBranch(theirBranchName, commitOid);
          });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
        theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
        "be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

        return repository.createCommit(theirBranch.name(), theirSignature,
        theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
        "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

        return removeFileFromIndex(repository, theirFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), theirFileName));
      })
      .then(function() {
        return fse.writeFile(path.join(repository.workdir(), ourFileName),
        ourFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, ourFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
        "77867fc0bfeb3f80ab18a78c8d53aa3a06207047");

        return repository.createCommit(ourBranch.name(), ourSignature,
        ourSignature, "we made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
        "e7f37ee070837052937e24ad8ba66f6d83ae7941");

        return removeFileFromIndex(repository, ourFileName);
      })
      .then(function() {
        return fse.remove(path.join(repository.workdir(), ourFileName));
      })
      .then(function() {
        return repository.checkoutBranch(ourBranchName);
      })
      .then(function() {
        return Promise.all([
          repository.getReference(ourBranchName),
          repository.getReference(theirBranchName)
        ]);
      })
      .then(function(refs) {
        assert.equal(refs.length, 2);

        return Promise.all([
          NodeGit.AnnotatedCommit.fromRef(repository, refs[0]),
          NodeGit.AnnotatedCommit.fromRef(repository, refs[1])
        ]);
      })
      .then(function(annotatedCommits) {
        assert.equal(annotatedCommits.length, 2);

        var ourAnnotatedCommit = annotatedCommits[0];
        var theirAnnotatedCommit = annotatedCommits[1];

        assert.equal(ourAnnotatedCommit.id().toString(),
        "e7f37ee070837052937e24ad8ba66f6d83ae7941");
        assert.equal(theirAnnotatedCommit.id().toString(),
        "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

        return NodeGit.Rebase.init(repository, ourAnnotatedCommit,
          theirAnnotatedCommit, null, {
            signingCb: (commitContent) => ({
              code: NodeGit.Error.CODE.OK,
              field: "moose-sig",
              signedData: "A moose was here."
            })
          });
        })
        .then(function(newRebase) {
          rebase = newRebase;

          // there should only be 1 rebase operation to perform
          assert.equal(rebase.operationEntrycount(), 1);

          return rebase.next();
        })
        .then(function(rebaseOperation) {
          assert.equal(rebaseOperation.type(),
          NodeGit.RebaseOperation.REBASE_OPERATION.PICK);
          assert.equal(rebaseOperation.id().toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

          // Make sure we don't crash calling the signature CB
          // after collecting garbage.
          garbageCollect();

          return rebase.commit(null, ourSignature);
        })
        .then(function(commitOid) {
          assert.equal(commitOid.toString(),
          "617cd03370dd799f372e9dcfcd0b097aede1bd7f");

          // git_rebase_operation_current returns the index of the rebase
          // operation that was last applied, so after the first operation, it
          // should be 0.
          assert.equal(rebase.operationCurrent(), 0);

          return rebase.finish(ourSignature, {});
        })
        .then(function(result) {
          assert.equal(result, 0);

          return repository.getBranchCommit(ourBranchName);
        })
        .then(function(commit) {
          // verify that the "ours" branch has moved to the correct place
          assert.equal(commit.id().toString(),
          "617cd03370dd799f372e9dcfcd0b097aede1bd7f");

          return Promise.all([
            commit.parent(0),
            NodeGit.Commit.extractSignature(
              repository,
              "617cd03370dd799f372e9dcfcd0b097aede1bd7f",
              "moose-sig"
            )
          ]);
        })
        .then(function([parent, { signature }]) {
          // verify that we are on top of "their commit"
          assert.equal(parent.id().toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");
          assert.equal(signature, "A moose was here.");
        });
      });

      it("can optionally skip signing commits", function() {
        var baseFileName = "baseNewFile.txt";
        var ourFileName = "ourNewFile.txt";
        var theirFileName = "theirNewFile.txt";

        var baseFileContent = "How do you feel about Toll Roads?";
        var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
        var theirFileContent = "I'm skeptical about Toll Roads";

        var ourSignature = NodeGit.Signature.create
        ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
        var theirSignature = NodeGit.Signature.create
        ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

        var repository = this.repository;
        var ourCommit;
        var ourBranch;
        var theirBranch;
        var rebase;

        return fse.writeFile(path.join(repository.workdir(), baseFileName),
        baseFileContent)
        // Load up the repository index and make our initial commit to HEAD
        .then(function() {
          return RepoUtils.addFileToIndex(repository, baseFileName);
        })
        .then(function(oid) {
          assert.equal(oid.toString(),
          "b5cdc109d437c4541a13fb7509116b5f03d5039a");

          return repository.createCommit("HEAD", ourSignature,
          ourSignature, "initial commit", oid, []);
        })
        .then(function(commitOid) {
          assert.equal(commitOid.toString(),
          "be03abdf0353d05924c53bebeb0e5bb129cda44a");

          return repository.getCommit(commitOid).then(function(commit) {
            ourCommit = commit;
          }).then(function() {
            return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
          });
        })
        .then(function(branch) {
          theirBranch = branch;
          return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
        })
        .then(function() {
          return RepoUtils.addFileToIndex(repository, theirFileName);
        })
        .then(function(oid) {
          assert.equal(oid.toString(),
          "be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

          return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
        })
        .then(function(commitOid) {
          assert.equal(commitOid.toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

          return removeFileFromIndex(repository, theirFileName);
        })
        .then(function() {
          return fse.remove(path.join(repository.workdir(), theirFileName));
        })
        .then(function() {
          return fse.writeFile(path.join(repository.workdir(), ourFileName),
          ourFileContent);
        })
        .then(function() {
          return RepoUtils.addFileToIndex(repository, ourFileName);
        })
        .then(function(oid) {
          assert.equal(oid.toString(),
          "77867fc0bfeb3f80ab18a78c8d53aa3a06207047");

          return repository.createCommit(ourBranch.name(), ourSignature,
          ourSignature, "we made a commit", oid, [ourCommit]);
        })
        .then(function(commitOid) {
          assert.equal(commitOid.toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");

          return removeFileFromIndex(repository, ourFileName);
        })
        .then(function() {
          return fse.remove(path.join(repository.workdir(), ourFileName));
        })
        .then(function() {
          return repository.checkoutBranch(ourBranchName);
        })
        .then(function() {
          return Promise.all([
            repository.getReference(ourBranchName),
            repository.getReference(theirBranchName)
          ]);
        })
        .then(function(refs) {
          assert.equal(refs.length, 2);

          return Promise.all([
            NodeGit.AnnotatedCommit.fromRef(repository, refs[0]),
            NodeGit.AnnotatedCommit.fromRef(repository, refs[1])
          ]);
        })
        .then(function(annotatedCommits) {
          assert.equal(annotatedCommits.length, 2);

          var ourAnnotatedCommit = annotatedCommits[0];
          var theirAnnotatedCommit = annotatedCommits[1];

          assert.equal(ourAnnotatedCommit.id().toString(),
          "e7f37ee070837052937e24ad8ba66f6d83ae7941");
          assert.equal(theirAnnotatedCommit.id().toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

          return NodeGit.Rebase.init(repository, ourAnnotatedCommit,
            theirAnnotatedCommit, null, {
              signingCb: () => ({
                code: NodeGit.Error.CODE.PASSTHROUGH
              })
            });
          })
          .then(function(newRebase) {
            rebase = newRebase;

            // there should only be 1 rebase operation to perform
            assert.equal(rebase.operationEntrycount(), 1);

            return rebase.next();
          })
          .then(function(rebaseOperation) {
            assert.equal(rebaseOperation.type(),
            NodeGit.RebaseOperation.REBASE_OPERATION.PICK);
            assert.equal(rebaseOperation.id().toString(),
            "e7f37ee070837052937e24ad8ba66f6d83ae7941");

            // Make sure we don't crash calling the signature CB
            // after collecting garbage.
            garbageCollect();

            return rebase.commit(null, ourSignature);
          })
          .then(function(commitOid) {
            assert.equal(commitOid.toString(),
            "b937100ee0ea17ef20525306763505a7fe2be29e");

            // git_rebase_operation_current returns the index of the rebase
            // operation that was last applied, so after the first operation, it
            // should be 0.
            assert.equal(rebase.operationCurrent(), 0);

            return rebase.finish(ourSignature, {});
          })
          .then(function(result) {
            assert.equal(result, 0);

            return repository.getBranchCommit(ourBranchName);
          })
          .then(function(commit) {
            // verify that the "ours" branch has moved to the correct place
            assert.equal(commit.id().toString(),
            "b937100ee0ea17ef20525306763505a7fe2be29e");

            return commit.parent(0);
          })
          .then(function(parent) {
            // verify that we are on top of "their commit"
            assert.equal(parent.id().toString(),
            "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");
            return NodeGit.Commit.extractSignature(
              repository,
              "b937100ee0ea17ef20525306763505a7fe2be29e",
              "moose-sig"
            )
            .then(function() {
              assert.fail("This commit should not be signed.");
            }, function (error) {
              if (error && error.message === "this commit is not signed") {
                return;
              }
              throw error;
            });
          });
        });

        it("will throw if commit signing cb returns an error code", function() {
          var baseFileName = "baseNewFile.txt";
          var ourFileName = "ourNewFile.txt";
          var theirFileName = "theirNewFile.txt";

          var baseFileContent = "How do you feel about Toll Roads?";
          var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
          var theirFileContent = "I'm skeptical about Toll Roads";

          var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
          var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

          var repository = this.repository;
          var ourCommit;
          var ourBranch;
          var theirBranch;
          var rebase;

          return fse.writeFile(path.join(repository.workdir(), baseFileName),
          baseFileContent)
          // Load up the repository index and make our initial commit to HEAD
          .then(function() {
            return RepoUtils.addFileToIndex(repository, baseFileName);
          })
          .then(function(oid) {
            assert.equal(oid.toString(),
            "b5cdc109d437c4541a13fb7509116b5f03d5039a");

            return repository.createCommit("HEAD", ourSignature,
            ourSignature, "initial commit", oid, []);
          })
          .then(function(commitOid) {
            assert.equal(commitOid.toString(),
            "be03abdf0353d05924c53bebeb0e5bb129cda44a");

            return repository.getCommit(commitOid).then(function(commit) {
              ourCommit = commit;
            }).then(function() {
              return repository.createBranch(ourBranchName, commitOid)
              .then(function(branch) {
                ourBranch = branch;
                return repository.createBranch(theirBranchName, commitOid);
              });
            });
          })
          .then(function(branch) {
            theirBranch = branch;
            return fse.writeFile(path.join(repository.workdir(), theirFileName),
            theirFileContent);
          })
          .then(function() {
            return RepoUtils.addFileToIndex(repository, theirFileName);
          })
          .then(function(oid) {
            assert.equal(oid.toString(),
            "be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

            return repository.createCommit(theirBranch.name(), theirSignature,
            theirSignature, "they made a commit", oid, [ourCommit]);
          })
          .then(function(commitOid) {
            assert.equal(commitOid.toString(),
            "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

            return removeFileFromIndex(repository, theirFileName);
          })
          .then(function() {
            return fse.remove(path.join(repository.workdir(), theirFileName));
          })
          .then(function() {
            return fse.writeFile(path.join(repository.workdir(), ourFileName),
            ourFileContent);
          })
          .then(function() {
            return RepoUtils.addFileToIndex(repository, ourFileName);
          })
          .then(function(oid) {
            assert.equal(oid.toString(),
            "77867fc0bfeb3f80ab18a78c8d53aa3a06207047");

            return repository.createCommit(ourBranch.name(), ourSignature,
            ourSignature, "we made a commit", oid, [ourCommit]);
          })
          .then(function(commitOid) {
            assert.equal(commitOid.toString(),
            "e7f37ee070837052937e24ad8ba66f6d83ae7941");

            return removeFileFromIndex(repository, ourFileName);
          })
          .then(function() {
            return fse.remove(path.join(repository.workdir(), ourFileName));
          })
          .then(function() {
            return repository.checkoutBranch(ourBranchName);
          })
          .then(function() {
            return Promise.all([
              repository.getReference(ourBranchName),
              repository.getReference(theirBranchName)
            ]);
          })
          .then(function(refs) {
            assert.equal(refs.length, 2);

            return Promise.all([
              NodeGit.AnnotatedCommit.fromRef(repository, refs[0]),
              NodeGit.AnnotatedCommit.fromRef(repository, refs[1])
            ]);
          })
          .then(function(annotatedCommits) {
            assert.equal(annotatedCommits.length, 2);

            var ourAnnotatedCommit = annotatedCommits[0];
            var theirAnnotatedCommit = annotatedCommits[1];

            assert.equal(ourAnnotatedCommit.id().toString(),
            "e7f37ee070837052937e24ad8ba66f6d83ae7941");
            assert.equal(theirAnnotatedCommit.id().toString(),
            "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");

            return NodeGit.Rebase.init(repository, ourAnnotatedCommit,
              theirAnnotatedCommit, null, {
                signingCb: () => ({
                  code: NodeGit.Error.CODE.ERROR
                })
              });
            })
            .then(function(newRebase) {
              rebase = newRebase;

              // there should only be 1 rebase operation to perform
              assert.equal(rebase.operationEntrycount(), 1);

              return rebase.next();
            })
            .then(function(rebaseOperation) {
              assert.equal(rebaseOperation.type(),
              NodeGit.RebaseOperation.REBASE_OPERATION.PICK);
              assert.equal(rebaseOperation.id().toString(),
              "e7f37ee070837052937e24ad8ba66f6d83ae7941");

              // Make sure we don't crash calling the signature CB
              // after collecting garbage.
              garbageCollect();

              return rebase.commit(null, ourSignature);
            })
            .then(function() {
              assert.fail("rebase.commit should have failed");
            }, function(error) {
              if (error && error.errno === NodeGit.Error.CODE.ERROR) {
                return;
              }
              throw error;
            });
          });
  }

    it("will not throw on patch already applied errors", function() {
      var baseFileName = "baseNewFile.txt";
      var theirFileName = "myFile.txt";

      var baseFileContent = "How do you feel about Toll Roads?";
      var theirFileContent = "Hello there";

      var ourSignature = NodeGit.Signature.create
            ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
      var theirSignature = NodeGit.Signature.create
            ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

      var repository = this.repository;
      var initialCommit;
      var ourBranch;
      var theirBranch;
      var rebase;

      return fse.writeFile(path.join(repository.workdir(), baseFileName),
        baseFileContent)
        // Load up the repository index and make our initial commit to HEAD
        .then(function() {
          return RepoUtils.addFileToIndex(repository, baseFileName);
        })
        .then(function(oid) {
          assert.equal(oid.toString(),
            "b5cdc109d437c4541a13fb7509116b5f03d5039a");

          return repository.createCommit("HEAD", ourSignature,
            ourSignature, "initial commit", oid, []);
        })
        .then(function(commitOid) {
          assert.equal(commitOid.toString(),
            "be03abdf0353d05924c53bebeb0e5bb129cda44a");

          return repository.getCommit(commitOid).then(function(commit) {
            initialCommit = commit;
          }).then(function() {
            return repository.createBranch(ourBranchName, commitOid)
              .then(function(branch) {
                ourBranch = branch;
                return repository.createBranch(theirBranchName, commitOid);
              });
          });
        })
        .then(function(branch) {
          theirBranch = branch;
          return fse.writeFile(
            path.join(repository.workdir(), theirFileName),
            theirFileContent
          );
        })
        .then(function() {
          return RepoUtils.addFileToIndex(repository, theirFileName);
        })
        .then(function(oid) {
          assert.equal(oid.toString(),
            "6f14d06b24fa8ea26f511dd8a94a003fd37eadc5");

          return repository.createCommit(theirBranch.name(), theirSignature,
            theirSignature, "they made a commit", oid, [initialCommit])
              .then(function(commitOid) {
                assert.equal(commitOid.toString(),
                  "c4cc225184b9c9682cb48294358d9d65f8ec42c7");
                return repository.createCommit(ourBranch.name(), ourSignature,
                  ourSignature, "we made a commit", oid, [initialCommit]);
              });
        })
        .then(function(commitOid) {
          assert.equal(commitOid.toString(),
            "5814ffa17b8a677191d89d5372f1e46d50d976ae");

          return removeFileFromIndex(repository, theirFileName);
        })
        .then(function() {
          return fse.remove(path.join(repository.workdir(), theirFileName));
        })
        .then(function() {
          return repository.checkoutBranch(ourBranchName);
        })
        .then(function() {
          return Promise.all([
            repository.getReference(ourBranchName),
            repository.getReference(theirBranchName)
          ]);
        })
        .then(function(refs) {
          assert.equal(refs.length, 2);

          return Promise.all([
            NodeGit.AnnotatedCommit.fromRef(repository, refs[0]),
            NodeGit.AnnotatedCommit.fromRef(repository, refs[1])
          ]);
        })
        .then(function(annotatedCommits) {
          assert.equal(annotatedCommits.length, 2);

          var ourAnnotatedCommit = annotatedCommits[0];
          var theirAnnotatedCommit = annotatedCommits[1];

          assert.equal(ourAnnotatedCommit.id().toString(),
            "5814ffa17b8a677191d89d5372f1e46d50d976ae");
          assert.equal(theirAnnotatedCommit.id().toString(),
            "c4cc225184b9c9682cb48294358d9d65f8ec42c7");

          return NodeGit.Rebase.init(
            repository,
            ourAnnotatedCommit,
            theirAnnotatedCommit
          );
        })
        .then(function(newRebase) {
          rebase = newRebase;

          // there should only be 1 rebase operation to perform
          assert.equal(rebase.operationEntrycount(), 1);

          return rebase.next();
        })
        .catch(function(error) {
          assert.fail(error);

          throw error;
        })
        .then(function(rebaseOperation) {
          assert.equal(rebaseOperation.type(),
            NodeGit.RebaseOperation.REBASE_OPERATION.PICK);
          assert.equal(rebaseOperation.id().toString(),
            "5814ffa17b8a677191d89d5372f1e46d50d976ae");

          return rebase.commit(null, ourSignature);
        })
        .then(function() {
          assert.fail("Rebase should have failed.");
        }, function (error) {
          if (error && error.errno === NodeGit.Error.CODE.EAPPLIED) {
            return;
          }

          assert.fail(error);

          throw error;
        })
        .then(function() {
          return repository.continueRebase();
        })
        .then(function() {
          return rebase.next();
        })
        .catch(function(error) {
          assert.equal(error.errno, NodeGit.Error.CODE.ITEROVER);
        });
      });


  it("rebase signs correctly if rebaseOptions are re-used", function () {
    const ourFileName = "ourNewFile.txt";
    const theirFileName = "theirNewFile.txt";

    const ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    const theirFileContent = "I'm skeptical about Toll Roads";

    const ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    const theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    let ourCommit;
    let theirCommit;

    let rebase;
    let newCommitOid;

    const rebaseOptions = {
      signingCb: () => ({
        code: NodeGit.Error.CODE.OK,
        field: "moose-sig",
        signedData: "A moose was here."
      })
    };

    const repository = this.repository;

    // Create two commits on master
    // one
    return fse.writeFile(path.join(repository.workdir(), ourFileName),
      ourFileContent)
      .then(() => RepoUtils.addFileToIndex(repository, ourFileName))
      .then((oid) => {
        assert.equal(oid.toString(),
          "11ead82b1135b8e240fb5d61e703312fb9cc3d6a");
        return repository.createCommit("HEAD", ourSignature, ourSignature,
          "we made a commit", oid, []);
      })
      .then((commitOid) => {
        assert.equal(commitOid.toString(),
          "91a183f87842ebb7a9b08dad8bc2473985796844");
        return repository.getCommit(commitOid);
      })
      .then((_ourCommit) => {
        ourCommit = _ourCommit;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
      })
      .then(() => RepoUtils.addFileToIndex(repository, theirFileName))
      .then((oid) => {
        assert.equal(oid.toString(),
          "76631cb5a290dafe2959152626bb90f2a6d8ec94");
        return repository.createCommit("HEAD", theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then((commitOid) => {
        assert.equal(commitOid.toString(),
          "0e9231d489b3f4303635fc4b0397830da095e7e7");
        return repository.getCommit(commitOid);

      })
      .then((_theirCommit) => {
        theirCommit = _theirCommit;
        return Promise.all([
          NodeGit.AnnotatedCommit.lookup(
            repository,
            ourCommit.id()
          ),
          NodeGit.AnnotatedCommit.lookup(
            repository,
            theirCommit.id()
          )
        ]);
      })
      // rebase latest commit
      .then(([ourAnnotatedCommit, theirAnnotatedCommit]) =>
        NodeGit.Rebase.init(
        repository,
        // branch, upstream, onto
        theirAnnotatedCommit, ourAnnotatedCommit, null,
        rebaseOptions // use once
      ))
      .then(() => {
        return NodeGit.Rebase.open(
          repository,
          rebaseOptions // use twice
        );
      })
      .then((_rebase) => {
        rebase = _rebase;
        return rebase.next();
      })
      .then(() => {
        const operationCurrentIndex = rebase.operationCurrent();
        assert(operationCurrentIndex === 0);
        // Make sure we don't crash calling the signature CB
        // after collecting garbage.
        garbageCollect();
        return rebase.commit(null, ourSignature);
      })
      .then((_newCommitOid) => {
        newCommitOid = _newCommitOid;
        assert.strictEqual(newCommitOid.toString(),
          "9909e435b52322a71dc341d747b29c392a34c745");
        return rebase.next();
      })
      .then(() => {
        assert.fail("should throw");
      })
      .catch((error) => {
        assert(error.errno === NodeGit.Error.CODE.ITEROVER);
        assert.strictEqual(rebase.finish(ourSignature), 0);
        return NodeGit.Commit.extractSignature(
          repository,
          newCommitOid.toString(),
          "moose-sig"
        );
      })
      .then((sig) => {
        assert.strictEqual(sig.signature, "A moose was here.");
      });
});
});
