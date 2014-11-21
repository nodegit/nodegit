var assert = require("assert");
var path = require("path");
var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));

var NodeGit = require("../../");
var Repository = NodeGit.Repository;

describe("Commit", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");
  var oid = "fce88902e66c72b5b93e75bdb5ae717038b221f6";

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;

      return repository.getCommit(oid).then(function(commit) {
        test.commit = commit;
      });
    });
  });

  it("will fail with an invalid sha", function() {
    return this.repository.getCommit("invalid").then(null, function(err) {
      assert.ok(err instanceof Error);
    });
  });

  it("has a message", function() {
    assert.equal(this.commit.message(), "Update README.md");
  });

  it("has a sha", function() {
    assert.equal(this.commit.sha(), oid);
  });

  it("has a time", function() {
    assert.equal(this.commit.timeMs(), 1362012884000);
  });

  it("has a date", function() {
    assert.equal(this.commit.date().getTime(), 1362012884000);
  });

  it("has a time offset", function() {
    assert.equal(this.commit.timeOffset(), 780);
  });

  it("can create a commit", function() {
    var expectedCommitId = "315e77328ef596f3bc065d8ac6dd2c72c09de8a5";
    var fileName = "newfile.txt";
    var fileContent = "hello world";

    var repo;
    var index;
    var treeOid;
    var parent;

    return NodeGit.Repository.open(reposPath)
    .then(function(repoResult) {
      repo = repoResult;
      return fse.writeFile(path.join(repo.workdir(), fileName), fileContent);
    })
    .then(function() {
      return repo.openIndex();
    })
    .then(function(indexResult) {
      index = indexResult;
      return index.read(1);
    })
    .then(function() {
      return index.addByPath(fileName);
    })
    .then(function() {
      return index.write();
    })
    .then(function() {
      return index.writeTree();
    })
    .then(function(oidResult) {
      treeOid = oidResult;
      return NodeGit.Reference.nameToId(repo, "HEAD");
    })
    .then(function(head) {
      return repo.getCommit(head);
    })
    .then(function(parentResult) {
      parent = parentResult;

      return Promise.all([
        NodeGit.Signature.create("Foo Bar", "foo@bar.com", 123456789, 60),
        NodeGit.Signature.create("Foo A Bar", "foo@bar.com", 987654321, 90)
      ]);
    })
    .then(function(signatures){
      var author = signatures[0];
      var committer = signatures[1];

      return repo.createCommit(
        "HEAD",
        author,
        committer,
        "message",
        treeOid,
        [parent]);
    })
    .then(function(commitId) {
      assert.equal(expectedCommitId, commitId);
    }, function(rejected) {
      assert.fail();
    });
  });

  describe("author", function() {
    before(function() {
      this.author = this.commit.author();
    });

    it("is available", function() {
      assert.ok(this.author instanceof NodeGit.Signature);
    });

    it("has a name", function() {
      assert.equal(this.author.name(), "Michael Robinson");
    });

    it("has an email", function() {
      assert.equal(this.author.email(), "mike@panmedia.co.nz");
    });
  });

  describe("committer", function() {
    before(function() {
      this.author = this.commit.committer();
    });

    it("is available", function() {
      assert.ok(this.author instanceof NodeGit.Signature);
    });

    it("has a name", function() {
      assert.equal(this.author.name(), "Michael Robinson");
    });

    it("has an email", function() {
      assert.equal(this.author.email(), "mike@panmedia.co.nz");
    });
  });

  it("can walk its repository's history", function(done) {
    var historyCount = 0;
    var expectedHistoryCount = 364;

    var history = this.commit.history();

    history.on("commit", function(commit) {
      historyCount++;
    });

    history.on("end", function(commits) {
      assert.equal(historyCount, expectedHistoryCount);
      assert.equal(commits.length, expectedHistoryCount);

      done();
    });

    history.on("error", function(err) {
      assert.ok(false);
    });

    history.start();
  });

  it("can fetch the master branch HEAD", function() {
    var repository = this.repository;

    return repository.getBranchCommit("master").then(function(commit) {
      return repository.getCommit(commit.sha());
    });
  });

  it("can fetch all its parents", function() {
    return this.commit.getParents().then(function(parents) {
      assert.equal(parents.length, 1);

      var sha = parents[0].sha();
      assert.equal(sha, "ecfd36c80a3e9081f200dfda2391acadb56dac27");
    });
  });

  it("can specify a parents limit", function() {
    return this.commit.getParents(0).then(function(parents) {
      assert.equal(parents.length, 0);
    });
  });

  it("can retrieve and walk a commit tree", function() {
    var commitTreeEntryCount = 0;
    var expectedCommitTreeEntryCount = 198;

    return this.commit.getTree().then(function(tree) {
      return new Promise(function(resolve, fail) {

        var treeWalker = tree.walk();

        treeWalker.on("entry", function(entry) {
          commitTreeEntryCount++;
        });

        treeWalker.on("error", function(error) {
          fail(error);
        });

        treeWalker.on("end", function(entries) {
          try {
            assert.equal(commitTreeEntryCount, expectedCommitTreeEntryCount);
            resolve();
          }
          catch (e) {
            fail(e);
          }
        });

        treeWalker.start();
      });
    });
  });

  it("can get the commit diff", function() {
    return this.commit.getDiff().then(function(diff) {
      assert.equal(diff.length, 1);
    });
  });
});
