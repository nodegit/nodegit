var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Submodule", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Submodule = NodeGit.Submodule;

  var repoPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(repoPath)
      .then(function(repo) {
        test.repository = repo;
      });
  });

  it("can walk over the submodules", function() {
    var repo = this.repository;
    var submoduleName = "vendor/libgit2";
    return repo.getSubmoduleNames()
      .then(function(submodules) {
        assert.equal(submodules.length, 1);

        var submodule = submodules[0];
        assert.equal(submodule, submoduleName);
        return submodule;
      })
      .then(function(submodule) {
        return Submodule.lookup(repo, submodule);
      })
      .then(function(submodule) {
        assert.equal(submodule.name(), submoduleName);
      });
  });
});
