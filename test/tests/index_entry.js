var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("IndexEntry", function() {
  var NodeGit = require("../../");
  var IndexEntry = NodeGit.IndexEntry;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return NodeGit.Repository.open(reposPath)
      .then(function(repo) {
        test.repository = repo;
        return repo.openIndex();
      });
  });

  it("can be initialized safely", function() {
    var entry = new IndexEntry();

    assert.equal(entry.path, "");
  });
});
