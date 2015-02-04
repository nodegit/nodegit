var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Branch", function() {
  var reposPath = local("../repos/workdir/.git");

  var Repository = require(local("../../lib/repository"));
  var Branch = require(local("../../lib/branch"));
  var branchName = "test-branch";
  var fullBranchName = "refs/heads/" + branchName;

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repo = repository;
    });
  });

  beforeEach(function() {
    var test = this;
    var repo = test.repo;

    return repo.getMasterCommit().then(function(masterCommit) {
      test.masterCommit = masterCommit;

      return repo.createBranch(branchName, masterCommit, true)
      .then(function(branch) {
        test.branch = branch;
      });
    });
  });

  it("can create a branch", function() {
    var branch = this.branch;
    var masterCommit = this.masterCommit;

    assert.equal(branch.name(), fullBranchName);
    assert.equal(branch.target().toString(), masterCommit.sha());
  });

  it("can delete a branch", function() {
    var repo = this.repo;

    Branch.delete(this.branch);

    return repo.getBranch(branchName).then(function() {
      // branch wasn't deleted
      assert.ok(false);
    }, function() {
      // branch was deleted!
      assert.ok(true);
    });
  });

  it("can see if the branch is pointed to by head", function() {
    var repo = this.repo;

    return repo.getBranch("master").then(function(branch) {
      assert.ok(branch.isHead());
    });
  });
});
