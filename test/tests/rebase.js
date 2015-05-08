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
        return repository.openIndex()
          .then(function(index) {
            index.read(1);
            index.addByPath(ourFileName);
            index.write();

            return index.writeTree();
          });
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
        return repository.openIndex()
          .then(function(index) {
            index.read(1);
            index.addByPath(theirFileName);
            index.write();

            return index.writeTree();
          });
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
        return repository.openIndex()
          .then(function(index) {
            index.read(1);
            index.removeByPath(theirFileName);
            index.write();

            return index.writeTree();
          });
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
});
