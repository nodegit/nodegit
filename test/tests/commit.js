var assert = require("assert");
var rimraf = require("rimraf");
var path = require("path");
var fs = require( "fs" );

var nodegit = require("../../");
var Repository = nodegit.Repository;

describe("Commit", function() {
  var historyCountKnownSHA = "fce88902e66c72b5b93e75bdb5ae717038b221f6";
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Commit = require("./commit");

  describe("when fetched", function() {

    it("makes its message available", function(done) {
      Repository.open(reposPath, function(error, repository) {
        repository.getCommit(historyCountKnownSHA, function(error, commit) {
          var message = commit.message();

          assert.equal(error, null);
          assert.equal(message, "Update README.md");

          done();
        });
      });
    });

    it("makes its sha available", function(done) {
      Repository.open(reposPath, function(error, repository) {
        repository.getCommit(historyCountKnownSHA, function(error, commit) {
          var sha = commit.sha();

          assert.equal(error, null);
          assert.equal(sha, historyCountKnownSHA);

          done();
        });
      });
    });

    it("makes its time available", function(done) {
      Repository.open(reposPath, function(error, repository) {
        repository.getCommit(historyCountKnownSHA, function(error, commit) {
          var time = commit.timeMs();

          assert.equal(error, null);
          assert.equal(time, 1362012884000);

          done();
        });
      });
    });

    it("makes its date available", function(done) {
      Repository.open(reposPath, function(error, repository) {
        repository.getCommit(historyCountKnownSHA, function(error, commit) {
          var date = commit.date();

          assert.equal(error, null);
          assert.equal(date.getTime(), 1362012884000);

          done();
        });
      });
    });

    it("makes its offset available", function(done) {
      Repository.open(reposPath, function(error, repository) {
        repository.getCommit(historyCountKnownSHA, function(error, commit) {
          var offset = commit.offset();

          assert.equal(error, null);
          assert.equal(offset, 780);

          done();
        });
      });
    });

  });
});
