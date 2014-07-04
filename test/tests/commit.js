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

  it("makes its message available", function() {
    assert.equal(this.commit.message(), "Update README.md");
  });

  it("makes its sha available", function() {
    assert.equal(this.commit.sha(), historyCountKnownSHA);
  });

  it("makes its time available", function() {
    assert.equal(this.commit.timeMs(), 1362012884000);
  });

  it("makes its date available", function() {
    assert.equal(this.commit.date().getTime(), 1362012884000);
  });

  it("makes its offset available", function() {
    assert.equal(this.commit.offset(), 780);
  });
});
