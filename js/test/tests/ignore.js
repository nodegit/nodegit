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
    function expectIgnoreState(repo, fileName, expected) {
      return Ignore.pathIsIgnored(repo, fileName)
        .then(function(ignored) {
          assert.equal(ignored, expected);
        });
    }

    return Promise.all([
      expectIgnoreState(this.repository, ".git", true),
      expectIgnoreState(this.repository, "LICENSE", false)
    ]);
  });
});
