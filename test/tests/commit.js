var assert = require("assert");
var rimraf = require("rimraf");
var path = require("path");
var fs = require( "fs" );

var nodegit = require("../../");
var Repository = nodegit.Repository;

describe("Commit", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");
  var historyCountKnownSHA = "fce88902e66c72b5b93e75bdb5ae717038b221f6";

  var Commit = require("./commit");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;

      return repository.getCommit(historyCountKnownSHA).then(function(commit) {
        test.commit = commit;
      });
    });
  });

  it("will fail with an invalid sha", function() {
    return this.repository.getCommit("invalid").then(null, function(error) {
      assert.ok(error instanceof Error);
    });
  });

  it("has a message", function() {
    assert.equal(this.commit.message(), "Update README.md");
  });

  it("has a sha", function() {
    assert.equal(this.commit.sha(), historyCountKnownSHA);
  });

  it("has a time", function() {
    assert.equal(this.commit.timeMs(), 1362012884000);
  });

  it("has a date", function() {
    assert.equal(this.commit.date().getTime(), 1362012884000);
  });

  it("has an offset", function() {
    assert.equal(this.commit.offset(), 780);
  });

  describe("author", function() {
    before(function() {
      this.author = this.commit.author();
    });

    it("is available", function() {
      assert.ok(this.author instanceof nodegit.Signature);
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
      assert.ok(this.author instanceof nodegit.Signature);
    });

    it("has a name", function() {
      assert.equal(this.author.name(), "Michael Robinson");
    });

    it("has an email", function() {
      assert.equal(this.author.email(), "mike@panmedia.co.nz");
    });
  });
});
