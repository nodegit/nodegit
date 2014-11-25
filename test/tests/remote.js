var assert = require("assert");
var path = require("path");

describe("Remote", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");

  var NodeGit = require("../../");
  var Repository = require("../../lib/repository");
  var Remote = require("../../lib/remote");

  function removeOrigins(repository) {
    return Remote.load(repository, "origin1").then(function(remote) {
      remote.delete();

      return Remote.load(repository, "origin2").then(function(remote) {
        remote.delete();
      });
    }).catch(function() {});
  }

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;


      return Remote.load(repository, "origin").then(function(remote) {
        test.remote = remote;

        return removeOrigins(repository);
      });
    });
  });

  after(function() {
    return removeOrigins(this.repository);
  });

  it("can load a remote", function() {
    assert.ok(this.remote instanceof Remote);
  });

  it("can read the remote url", function() {
    assert.equal(
      this.remote.url().replace(".git", ""),
      "https://github.com/nodegit/test");
  });

  it("can push a remote", function() {
    assert.equal(this.remote.pushurl(), undefined);
  });

  it("can set a remote", function() {
    var repository = this.repository;
    var url = "https://github.com/nodegit/nodegit";

    return Remote.create(repository, "origin1", url).then(function(remote) {
      return remote.setPushurl("https://google.com/", function() {
        assert(remote.pushurl(), "https://google.com/");
      });
    });
  });

  it("can read the remote name", function() {
    assert.equal(this.remote.name(), "origin");
  });

  it("can create a new remote", function() {
    var repository = this.repository;
    var url = "https://github.com/nodegit/test";

    return Remote.create(repository, "origin2", url).then(function() {
      return Remote.load(repository, "origin2").then(function(remote) {
        assert(remote.url(), "https://github.com/nodegit/test");
      });
    });
  });

  it("can delete a remote", function() {
    var repository = this.repository;
    var url = "https://github.com/nodegit/test";

    return Remote.create(repository, "origin3", url).then(function() {
      return Remote.load(repository, "origin3").then(function(remote) {
        remote.delete();
      });
    });
  });

  it("can download from a remote", function() {
    var repo = this.repository;

    return Remote.load(repo, "origin")
    .then(function(remote) {
      remote.connect(NodeGit.Enums.DIRECTION.FETCH);
      return remote.download();
    })
    .then(function() {
      assert(true);
    }, function() {
      assert(false);
    });
  });

  it("can fetch from a remote", function() {
    return this.repository.fetch("origin")
    .then(function() {
      assert(true);
    }, function() {
      assert(false);
    });
  });
});
