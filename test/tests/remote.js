var assert = require("assert");
var path = require("path");

describe("Remote", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Repository = require("../../lib/repository");
  var Remote = require("../../lib/remote");

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;

      return Remote.load(repository, "origin").then(function(remote) {
        test.remote = remote;
      });
    });
  });

  after(function() {
    return Remote.load(this.repository, "origin2").then(function(remote) {
      remote.delete();
    });
  });

  it("can load a remote", function() {
    assert.ok(this.remote instanceof Remote);
  });

  it("can read the remote url", function() {
    assert.equal(
      this.remote.url().replace(".git", ""),
      "https://github.com/nodegit/nodegit");
  });

  it("can read the remote name", function() {
    assert.equal(this.remote.name(), "origin");
  });

  it("can create a new remote", function() {
    var repository = this.repository;
    var url = "https://github.com/nodegit/nodegit";

    return Remote.create(repository, "origin2", url).then(function() {
      return Remote.load(repository, "origin2").then(function(remote) {
        assert(remote.url(), "https://github.com/nodegit/nodegit");
      });
    });
  });

  it("can delete a remote", function() {
    var repository = this.repository;
    var url = "https://github.com/nodegit/nodegit";

    return Remote.create(repository, "origin3", url).then(function() {
      return Remote.load(repository, "origin3").then(function(remote) {
        remote.delete();
      });
    });
  });
});
