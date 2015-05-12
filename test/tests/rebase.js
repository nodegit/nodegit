var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));

describe("Rebase", function() {
  var NodeGit = require("../../");

  var repoPath = local("../repos/rebase");
  var ourBranchName = "ours";
  var theirBranchName = "theirs";

  var addFileToIndex = function(repository, fileName) {
    return repository.openIndex()
      .then(function(index) {
        index.read(1);
        index.addByPath(fileName);
        index.write();

        return index.writeTree();
      });
  };

  var removeFileFromIndex = function(repository, fileName) {
    return repository.openIndex()
      .then(function(index) {
        index.read(1);
        index.removeByPath(fileName);
        index.write();

        return index.writeTree();
      });
  };

  beforeEach(function() {
    var test = this;
    return fse.remove(repoPath)
      .then(function() {
        return fse.ensureDir(repoPath);
      })
      .then(function() {
        return NodeGit.Repository.init(repoPath, 0);
      })
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
        return addFileToIndex(repository, ourFileName);
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
        return addFileToIndex(repository, theirFileName);
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
          theirAnnotatedCommit, theirAnnotatedCommit, ourSignature,
          new NodeGit.RebaseOptions());
      })
      .then(function(rebase) {
        assert.equal(rebase.operationEntrycount(), 0);
        assert.equal(rebase.operationCurrent(), 0);

        return rebase.finish(ourSignature, new NodeGit.RebaseOptions());
      })
      .then(function() {
        return repository.getBranchCommit(ourBranchName);
      })
      .then(function(commit) {
        assert.equal(commit.id().toString(),
          "0e9231d489b3f4303635fc4b0397830da095e7e7");
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
    var theirCommitOid;
    var ourBranch;
    var theirBranch;
    var rebase;

    return fse.writeFile(path.join(repository.workdir(), baseFileName),
      baseFileContent)
      // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return addFileToIndex(repository, baseFileName);
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
        return addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        theirCommitOid = commitOid;
        assert.equal(commitOid.toString(),
          "e9ebd92f2f4778baf6fa8e92f0c68642f931a554");
      })
      .then(function() {
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
        return addFileToIndex(repository, ourFileName);
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
      })
      .then(function() {
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
          theirAnnotatedCommit, null, ourSignature, null);
      })
      .then(function(newRebase) {
        rebase = newRebase;

        // there should only be 1 rebase operation to perform
        assert.equal(rebase.operationEntrycount(), 1);

        var opts = new NodeGit.CheckoutOptions();
        opts.checkoutStrategy = NodeGit.Checkout.STRATEGY.SAFE_CREATE;

        return rebase.next(opts);
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

        return rebase.finish(ourSignature, new NodeGit.RebaseOptions());
      })
      .then(function(result) {
        assert.equal(result, 0);

        return repository.getBranchCommit(ourBranchName);
      })
      .then(function(commit) {
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
});
