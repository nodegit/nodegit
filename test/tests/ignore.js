var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Ignore", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Ignore = NodeGit.Ignore;

  var reposPath = local("../repos/workdir");

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;
    });
  });

  it("can determine if a path is ignored", function() {
    assert.equal(Ignore.pathIsIgnored(this.repository, ".git"), true);
    assert.equal(Ignore.pathIsIgnored(this.repository, "LICENSE"), false);
  });
});
