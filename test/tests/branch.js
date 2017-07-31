var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Branch", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Branch = NodeGit.Branch;
  var AnnotatedCommit = NodeGit.AnnotatedCommit;
  var branchName = "test-branch";
  var branchName2 = "test-branch2";
  var fullBranchName = "refs/heads/" + branchName;
  var fullBranchName2 = "refs/heads/" + branchName2;
  var remoteName = "origin";
  var upstreamName = "origin/master";
  var fullUpstreamName = "refs/remotes/origin/master";
  var nonHeadCommit = "c82fb078a192ea221c9f1093c64321c60d64aa0d";

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
        return test.repository.createBranch(
          branchName2, test.masterCommit, true);
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
      .then(Promise.reject.bind(Promise), Promise.resolve.bind(Promise));
  });

  it("can see if the branch is pointed to by head", function() {
    var repo = this.repository;

    return repo.getBranch("master")
      .then(function(branch) {
        assert.ok(branch.isHead());
      });
  });

  it("can set an upstream for a branch", function() {
    var branch = this.branch;

    return NodeGit.Branch.setUpstream(branch, upstreamName)
      .then(function() {
        return NodeGit.Branch.upstream(branch);
      })
      .then(function(upstream) {
        assert.equal(upstream.shorthand(), upstreamName);
      });
  });

  it("can get the name of a branch", function() {
    var branch = this.branch;

    return NodeGit.Branch.name(branch)
      .then(function(branchNameToTest) {
        assert.equal(branchNameToTest, branchName);
      });
  });

  it("can get the remote name of a branch", function() {
    var repo = this.repository;

    return NodeGit.Branch.remoteName(repo, fullUpstreamName)
      .then(function(remoteNameToTest) {
        assert.equal(remoteNameToTest, remoteName);
      });
  });

  it("cannot get remote name from a non-remote branch", function() {
    var repo = this.repository;

    return NodeGit.Branch.remoteName(repo, fullBranchName)
      .then(function() {
        assert.fail("The ref should not have been a remote");
      })
      .catch(function(err) {
        assert.strictEqual(err.errno, -1);
      });
  });

  it("can rename a branch", function() {
    var branch = this.branch;

    // don't force the move
    return Branch.move(branch, branchName2, 0)
      .then(function(branch) {
        return Promise.reject(new Error(
          "should not be able to rename the branch"));
      }, function(error) {
        return Promise.resolve()
          .then(function() {
            // force the move
            return Branch.move(branch, branchName2, 1);
          })
          .then(function(branch) {
            assert.equal(branch.name(), fullBranchName2);
          });
      });
  });

  it("can lookup a branch", function() {
    var repo = this.repository;

    return Branch.lookup(repo, branchName, Branch.BRANCH.LOCAL)
      .then(function(branch) {
        assert.equal(branch.name(), fullBranchName);
        return Branch.lookup(repo, upstreamName, Branch.BRANCH.REMOTE);
      })
      .then(function(branch) {
        assert.equal(branch.name(), fullUpstreamName);
      });
  });

  it("can create branch from annotated commit", function() {
    var repo = this.repository;
    var annotatedCommit = null;

    return AnnotatedCommit.fromRevspec(repo, nonHeadCommit)
      .then(function(theAnnotatedCommit) {
        annotatedCommit = theAnnotatedCommit;
        return Branch.createFromAnnotated(
          repo, branchName, annotatedCommit, 0);
      })
      .then(function(branch) {
        return Promise.reject(new Error(
          "should not be able to create the branch"));
      }, function(error) {
        return Promise.resolve()
          .then(function() {
            // force the branch creation
            return Branch.createFromAnnotated(
              repo, branchName, annotatedCommit, 1);
          })
          .then(function(branch) {
            assert.equal(branch.name(), fullBranchName);
          });
      });
  });
});
