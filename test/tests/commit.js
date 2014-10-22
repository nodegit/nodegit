var assert = require("assert");
var path = require("path");

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

    return repository.getBranch("master").then(function(branch) {
      return repository.getCommit(branch.sha());
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

  it("can retrieve and walk a commit tree", function(done) {
    var commitTreeEntryCount = 0;
    var expectedCommitTreeEntryCount = 198;

    this.commit.getTree().then(function(tree) {
      var treeWalker = tree.walk();

      treeWalker.on("entry", function(entry) {
        commitTreeEntryCount++;
      });

      treeWalker.on("error", function() {
        assert.ok(false);
      });

      treeWalker.on("end", function(entries) {
        assert.equal(commitTreeEntryCount, expectedCommitTreeEntryCount);
        done();
      });

      treeWalker.start();
    });
  });

  it("can get the commit diff", function() {
    return this.commit.getDiff().then(function(diff) {
      assert.equal(diff.length, 1);
    });
  });
});
