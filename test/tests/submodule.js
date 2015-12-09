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
    return Submodule.foreach(this.repository, function(submodule,
                                                       name, context) {
      assert.equal(name, "vendor/libgit2");
      assert.notEqual(submodule, null);
    });
  });
});
