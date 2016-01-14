var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Submodule", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var RepoUtils = require("../utils/repository_setup");
  var Submodule = NodeGit.Submodule;

  var repoPath = local("../repos/submodule");

  beforeEach(function() {
    var test = this;

    return RepoUtils.createRepository(repoPath)
      .then(function(repo) {
        test.repository = repo;
      });
  });

  it("can walk over the submodules", function() {
    var repo;
    var submoduleName = "vendor/libgit2";

    return Repository.open(local("../repos/workdir"))
      .then(function(_repo) {
        repo = _repo;
        return repo.getSubmoduleNames();
      })
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

  it("can setup and finalize submodule add", function() {
    var repo = this.repository;
    var submodulePath = "hellogitworld";
    var submoduleUrl = "https://github.com/githubtraining/hellogitworld.git";

    var submodule;
    var submoduleRepo;

    return NodeGit.Submodule.addSetup(repo, submoduleUrl, submodulePath, 0)
      .then(function(_submodule) {
        submodule = _submodule;

        return submodule.init(0);
      })
      .then(function() {
        return submodule.open();
      })
      .then(function(_submoduleRepo) {
        submoduleRepo = _submoduleRepo;
        return submoduleRepo.fetch("origin", null, null);
      })
      .then(function() {
        return submoduleRepo.getReference("origin/master");
      })
      .then(function(reference) {
        return reference.peel(NodeGit.Object.TYPE.COMMIT);
      })
      .then(function(commit) {
        return submoduleRepo.createBranch("master", commit);
      })
      .then(function() {
        return submodule.addFinalize();
      })
      .then(function() {
        // check whether the submodule exists
        return Submodule.lookup(repo, submodulePath);
      })
      .then(function(submodule) {
        assert.equal(submodule.name(), submodulePath);
        // check whether .gitmodules and the submodule are in the index
        return repo.openIndex();
      })
      .then(function(index) {
        var entries = index.entries();
        assert.equal(entries.length, 2);
        assert.equal(entries[0].path, ".gitmodules");
        assert.equal(entries[1].path, submodulePath);
      });
  });
});
