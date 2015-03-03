var assert = require("assert");
var path = require("path");
var Promise = require("nodegit-promise");
var local = path.join.bind(path, __dirname);

describe("Branch", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Branch = NodeGit.Branch;
  var branchName = "test-branch";
  var fullBranchName = "refs/heads/" + branchName;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
        return repository.getMasterCommit();
      })
      .then(function(masterCommit) {
        test.masterCommit = masterCommit;

        return test.repository.createBranch(branchName, masterCommit, true);
      })
      .then(function(branch) {
        test.branch = branch;
      });
  });

  it("can create a branch", function() {
    var branch = this.branch;
    var masterCommit = this.masterCommit;

    assert.equal(branch.name(), fullBranchName);
    assert.equal(branch.target().toString(), masterCommit.sha());
  });

  it("can delete a branch", function() {
    var repo = this.repository;

    Branch.delete(this.branch);

    return repo.getBranch(branchName)
      // Reverse the results, since if we found it it wasn't deleted
      .then(Promise.reject, Promise.resolve);
  });

  it("can see if the branch is pointed to by head", function() {
    var repo = this.repository;

    return repo.getBranch("master")
      .then(function(branch) {
        assert.ok(branch.isHead());
      });
  });
});
