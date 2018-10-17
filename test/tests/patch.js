var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Patch", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;

  var reposPath = local("../repos/workdir");
  var oid = "fce88902e66c72b5b93e75bdb5ae717038b221f6";

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
        test.repository = repository;

        return repository.refreshIndex();
      })
      .then(function(index) {
        test.index = index;

        return test.repository.getBranchCommit("master");
      })
      .then(function(masterCommit) {
        return masterCommit.getTree();
      })
      .then(function(tree) {
        test.masterCommitTree = tree;

        return test.repository.getCommit(oid);
      })
      .then(function(commit) {
        test.commit = commit;

        return commit.getDiff();
      })
      .then(function(diff) {
        test.diff = diff;

        return diff[0].patches();
      })
      .catch(function(e) {
          return Promise.reject(e);
      });
  });

  it("retrieve the line stats of a patch", function() {
    return this.diff[0].patches()
      .then(function(patches) {
        var patch = patches[0];
        var lineStats = patch.lineStats();

        assert.equal(patch.oldFile().path(), "README.md");
        assert.equal(patch.newFile().path(), "README.md");
        assert.equal(patch.size(), 1);
        assert.ok(patch.isModified());
        assert.equal(lineStats.total_context, 3);
        assert.equal(lineStats.total_additions, 1);
        assert.equal(lineStats.total_deletions, 1);
      });

  });
});
