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
  var Status = NodeGit.Status;

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

    return Revert.revert(test.repository, test.firstCommit, new RevertOptions())
      .then(function() {
        try {
          fs.statSync(path.join(repoPath, fileName));
        } catch (e) {
          // we expect this not to exist
          return;
        }

        assert.fail("Working directory was not reverted");
      });
  });

  it("revert modifies the index", function() {
    return Revert.revert(test.repository, test.firstCommit, new RevertOptions())
      .then(() => test.repository.getStatus())
      .then((status) => {
        assert.equal(1, status.length);
        assert.ok(_.endsWith(fileName, status[0].path()));
        assert.equal(Status.STATUS.INDEX_DELETED, status[0].statusBit());
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
