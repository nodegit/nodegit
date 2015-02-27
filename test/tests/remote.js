var assert = require("assert");
var path = require("path");
var Promise = require("nodegit-promise");
var local = path.join.bind(path, __dirname);

describe("Remote", function() {
  var NodeGit = require(local("../../"));
  var Repository = require(local("../../lib/repository"));
  var Remote = require(local("../../lib/remote"));

  var reposPath = local("../repos/workdir/.git");
  var url = "https://github.com/nodegit/test";
  var url2 = "https://github.com/nodegit/test2";

  function removeOrigins(repository) {
    Remote.delete(repository, "origin1");
    Remote.delete(repository, "origin2");
    Remote.delete(repository, "origin3");
    Remote.delete(repository, "test2");
  }

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;

        return Remote.lookup(repository, "origin");
      })
      .then(function(remote) {
        test.remote = remote;

        return removeOrigins(test.repository);
      });
  });

  after(function() {
    return removeOrigins(this.repository);
  });

  it("can load a remote", function() {
    assert.ok(this.remote instanceof Remote);
  });

  it("can read the remote url", function() {
    assert.equal( this.remote.url().replace(".git", ""), url);
  });

  it("has an empty pushurl by default", function() {
    assert.equal(this.remote.pushurl(), undefined);
  });

  it("can set a remote", function() {
    var repository = this.repository;
    var newRemote;

    return Remote.create(repository, "origin1", url)
      .then(function(remote) {
        newRemote = remote;
        return remote.setPushurl("https://google.com/");
      })
      .then(function() {
        assert(newRemote.pushurl(), "https://google.com/");
      });
  });

  it("can read the remote name", function() {
    assert.equal(this.remote.name(), "origin");
  });

  it("can create and load a new remote", function() {
    var repository = this.repository;

    return Remote.create(repository, "origin2", url)
      .then(function() {
        return Remote.lookup(repository, "origin2");
      })
      .then(function(remote) {
        assert(remote.url(), url);
      });
  });

  it("can delete a remote", function() {
    var repository = this.repository;

    return Remote.create(repository, "origin3", url)
      .then(function(remote) {
        Remote.delete(repository, "origin3");
        return Remote.lookup(repository, "origin3");
      })
      .then(Promise.reject, Promise.resolve);
  });

  it("can download from a remote", function() {
    var repo = this.repository;

    return repo.getRemote("origin")
      .then(function(remote) {
        remote.setCallbacks({
          certificateCheck: function() {
            return 1;
          }
        });

        return remote.connect(NodeGit.Enums.DIRECTION.FETCH)
        .then(function() {
          return remote.download(null);
        }).then(function() {
          return remote.disconnect();
        });
      });
  });

  it("can monitor transfer progress while downloading", function() {
    // Set a reasonable timeout here now that our repository has grown.
    this.timeout(600000);

    var repo = this.repository;
    var wasCalled = false;

    return Remote.create(repo, "test2", url2)
    .then(function() {
      return repo.getRemote("test2");
    })
    .then(function(remote) {
      remote.setCallbacks({
        credentials: function(url, userName) {
          return NodeGit.Cred.sshKeyFromAgent(userName);
        },
        certificateCheck: function() {
          return 1;
        },

        transferProgress: function() {
          wasCalled = true;
        }
      });

      return remote.fetch(null, repo.defaultSignature(), null);
    })
    .then(function() {
      assert.ok(wasCalled);

      Remote.delete(repo, "test2");
    });
  });

  it("can fetch from a remote", function() {
    return this.repository.fetch("origin", {
      credentials: function(url, userName) {
        return NodeGit.Cred.sshKeyFromAgent(userName);
      },
      certificateCheck: function() {
        return 1;
      }
    });
  });

  it("can fetch from all remotes", function() {
    // Set a reasonable timeout here for the fetchAll test
    this.timeout(15000);

    return this.repository.fetchAll({
      credentials: function(url, userName) {
        return NodeGit.Cred.sshKeyFromAgent(userName);
      },
      certificateCheck: function() {
        return 1;
      }
    });
  });

});
