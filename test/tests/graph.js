var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Graph", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Graph = NodeGit.Graph;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      });
  });

  it("can get commits ahead/behind for 2 different commits", function() {
    return Graph.aheadBehind(
      this.repository,
      "32789a79e71fbc9e04d3eff7425e1771eb595150",
      "1729c73906bb8467f4095c2f4044083016b4dfde")
    .then(function(result) {
      assert.equal(result.ahead, 1);
      assert.equal(result.behind, 1);
    });
  });
});
