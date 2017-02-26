var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Revparse", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Revparse = NodeGit.Revparse;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;
    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
        return test.repository.getHeadCommit();
      })
      .then(function(commit) {
        test.commit = commit;
      });
  });

  it("can revparse HEAD commit with single method", function() {
    var test = this;
    return Revparse.single(this.repository, "HEAD")
      .then(function(headCommit) {
        assert.ok(headCommit.isCommit());
        assert.equal(headCommit.id().toString(), test.commit.id().toString());
      });
  });

  it("will fail on invalid spec", function() {
    return Revparse.single(this.repository, "INVALID")
      .then(function() {

      })
      .catch(function(error) {
        assert.ok(error instanceof Error);
        assert.equal(error.message, "revspec 'INVALID' not found");
      });
  });

  it("will fail without repo", function() {
    return Revparse.single("", "INVALID")
      .then(function() {

      })
      .catch(function(error) {
        assert.ok(error instanceof Error);
        assert.equal(error.message, "Repository repo is required.");
      });
  });

  it("will fail without spec", function() {
    return Revparse.single(this.repository)
      .then(function() {

      })
      .catch(function(error) {
        assert.ok(error instanceof Error);
        assert.equal(error.message, "String spec is required.");
      });
  });

});
