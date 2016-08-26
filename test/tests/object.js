var path = require("path");
var leakTest = require("../utils/leak_test");

var local = path.join.bind(path, __dirname);

describe("Commit", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;
    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      });
  });

  it("does not leak", function() {
    var test = this;
    var oid = "fce88902e66c72b5b93e75bdb5ae717038b221f6";

    return leakTest(NodeGit.Object, function() {
        return NodeGit.Object.lookup(test.repository, oid);
    });
  });
});
