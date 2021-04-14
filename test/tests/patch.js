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

  it("can generate patch from blobs", async function() {
    // Generates a patch for README.md from commit fce88902e66c72b5b93e75bdb5ae717038b221f6
    const file = "README.md";

    const blob = await NodeGit.Blob.lookup(this.repository, "b252f396b17661462372f78b7bcfc403b8731aaa");
    const oldBlob = await NodeGit.Blob.lookup(this.repository, "b8d014998072c3f9e4b7eba8486011e80d8de98a");
    const patch = await NodeGit.Patch.fromBlobs(oldBlob, file, blob, file);

    assert.strictEqual(patch.size(0, 0, 0), 254);
  });

  it ("can generate patch from blobs without 'old_blob'", async function() {
    // Generates a patch for README.md from commit fce88902e66c72b5b93e75bdb5ae717038b221f6
    // without old_blob. Should show all lines as additions.
    const file = "README.md";

    const blob = await NodeGit.Blob.lookup(this.repository, "b252f396b17661462372f78b7bcfc403b8731aaa");
    const patch = await NodeGit.Patch.fromBlobs(null, file, blob, file);

    assert.strictEqual(patch.size(0, 0, 0), 8905);
  });
});
