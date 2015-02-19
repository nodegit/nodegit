var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Reset", function() {
  var Repository = require(local("../../lib/repository"));
  var Reset = require(local("../../lib/reset"));

  var reposPath = local("../repos/workdir/.git");
  var currentCommitOid = "32789a79e71fbc9e04d3eff7425e1771eb595150";
  var previousCommitOid = "c82fb078a192ea221c9f1093c64321c60d64aa0d";
  var filePath = "package.json";

  before(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repo = repository;

        return test.repo.getCommit(currentCommitOid);
      })
      .then(function(commit) {
        test.currentCommit = commit;

        return commit.getEntry(filePath);
      })
      .then(function(entry) {
        return entry.getBlob();
      })
      .then(function(blob) {
        test.currentCommitBlob = blob;

        return test.repo.getCommit(previousCommitOid);
      })
      .then(function(commit) {
        test.previousCommit = commit;

        return commit.getEntry(filePath);
      })
      .then(function(entry) {
        return entry.getBlob();
      })
      .then(function(blob) {
        test.previousCommitBlob = blob;
      });
  });

  it("can reset a file to a previous commit", function() {
    var test = this;

    return Reset.default(test.repo, test.previousCommit, filePath)
    .then(function() {
      return test.repo.openIndex();
    })
    .then(function(index) {
      return index.writeTree();
    })
    .then(function(oid) {
      return test.repo.getTree(oid);
    })
    .then(function(tree) {
      return tree.getEntry(filePath);
    })
    .then(function(entry) {
      return entry.getBlob();
    })
    .then(function(blob) {
      var currentCommitContents = test.currentCommitBlob.toString();
      var previousCommitContents = test.previousCommitBlob.toString();
      var resetContents = blob.toString();

      assert(resetContents != currentCommitContents);
      assert(resetContents == previousCommitContents);
    });
  });
});
