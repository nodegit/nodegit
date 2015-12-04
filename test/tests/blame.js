var assert = require("assert");
var RepoUtils = require("../utils/repository_setup");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Blame", function() {
  var NodeGit = require("../../");

  var Blame = NodeGit.Blame;

  var test;
  var fileName = "foobar.js";
  var repoPath = local("../repos/blameRepo");

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
      });
  });

  it("can initialize blame without options", function() {
    return Blame.file(test.repository, fileName)
      .then(function(blame) {
        assert(blame);
      });
  });
});
