var assert = require("assert");
var path = require("path");

describe("Repository", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Repository = require("../../lib/repository");
  var Remote = require("../../lib/remote");

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;
    });
  });

  it("can read git remote url", function() {
    return Remote.load(this.repository, "origin").then(function(remote) {
      assert.ok(remote instanceof Remote);
      assert.equal(remote.url(), "https://github.com/nodegit/nodegit");
    });
  });
});
