var _ = require("lodash");
var assert = require("assert");
var RepoUtils = require("../utils/repository_setup");
var path = require("path");
var fs = require("fs");
var local = path.join.bind(path, __dirname);

describe("Revert", function() {
  var NodeGit = require("../../");

  var Revert = NodeGit.Revert;
  var RevertOptions = NodeGit.RevertOptions;

  var test;
  var fileName = "foobar.js";
  var repoPath = local("../repos/revertRepo");

  beforeEach(function() {
    test = this;

    return RepoUtils.createRepository(repoPath)
      .then(function(repository) {
        test.repository = repository;

        return RepoUtils.commitFileToRepo(
          repository,
          fileName,
          "line1\nline2\nline3"
        );
      })
      .then(function(firstCommit) {
        test.firstCommit = firstCommit;
      });
  });

  it("revert modifies the working directoy", function() {
    var fileStats = fs.statSync(path.join(repoPath, fileName));
    assert.ok(fileStats.isFile());

    Revert.revert(test.repository, test.firstCommit, new RevertOptions())
      .then(function() {
        try {
          fs.statSync(path.join(repoPath, fileName));
          assert.fail("Working directory was not reverted");
        }
        catch (error) {
          // pass
        }
      });
  });

  it("revert modifies the index", function() {
    Revert.revert(test.repository, test.firstCommit, new RevertOptions())
    .then(function() {
      return test.repository.index();
    })
    .then(function(index) {
      var entries = index.entries;
      assert.equal(1, entries.length);
      assert.ok(_.endsWith(fileName, entries[0].path));
    });
  });

  it("RevertOptions is optional (unspecified)", function() {
    return Revert.revert(test.repository, test.firstCommit)
      .catch(function(error) {
        throw error;
      });
  });

  it("RevertOptions is optional (null)", function() {
    return Revert.revert(test.repository, test.firstCommit, null)
      .catch(function(error) {
        throw error;
      });
  });

  it("RevertOptions without MergeOptions should not segfault", function() {
    return Revert.revert(test.repository, test.firstCommit, {})
      .catch(function(error) {
        throw error;
      });
  });
});
