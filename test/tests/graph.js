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

  it("can tell if a commit is a descendant of another", function() {
    return Graph.descendantOf(
      this.repository,
      "32789a79e71fbc9e04d3eff7425e1771eb595150",
      "e0aeedcff0584ebe00aed2c03c8ecd10839df908"
    )
      .then(function(result) {
        assert.equal(result, 1);
      });
  });

  it("can tell if a commit is not a descendant of another", function() {
    return Graph.descendantOf(
      this.repository,
      "1528a019c504c9b5a68dc7d83bb2a887eb2473af",
      "32789a79e71fbc9e04d3eff7425e1771eb595150"
    )
      .then(function(result) {
        assert.equal(result, 0);
      });
  });

  it("descendantOf will error if provided bad commits", function() {
    return Graph.descendantOf(
      this.repository,
      "81b06facd90fe7a6e9bbd9cee59736a79105b7be",
      "26744fc697849d370246749b67ac43b792a4af0c"
    )
      .catch(function(result) {
        assert(~result.message.indexOf("object not found - no match for id"));
      });
  });

  it("can tell if a commit is reachable from any of a list of commits", function() {
    return Graph.reachableFromAny(
      this.repository,
      "32789a79e71fbc9e04d3eff7425e1771eb595150",
      [
        "1729c73906bb8467f4095c2f4044083016b4dfde",
        "e0aeedcff0584ebe00aed2c03c8ecd10839df908"
      ]
    )
      .then(function(result) {
        assert.equal(result, 0);
      });
  });
});
