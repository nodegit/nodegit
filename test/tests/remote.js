var assert = require("assert");
var path = require("path");
var Promise = require("nodegit-promise");
var local = path.join.bind(path, __dirname);

describe("Remote", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Remote = NodeGit.Remote;

  var reposPath = local("../repos/workdir");
  var url = "https://github.com/nodegit/test";
  var url2 = "https://github.com/nodegit/test2";
  var privateUrl = "git@github.com:nodegit/private";

  function removeNonOrigins(repo) {
    return repo.getRemotes()
      .then(function(remotes) {
        return remotes.reduce(function(promise, remote) {
          if (remote !== "origin") {
            promise = promise.then(function() {
              return Remote.delete(repo, remote);
            });
          }

          return promise;
        }, Promise.resolve());
      });
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

        return removeNonOrigins(test.repository);
      });
  });

  after(function() {
    return removeNonOrigins(this.repository);
  });

  it("can load a remote", function() {
    assert.ok(this.remote instanceof Remote);
  });

  it("can read the remote url", function() {
    assert.equal(this.remote.url().replace(".git", ""), url);
  });

  it("has an empty pushurl by default", function() {
    assert.equal(this.remote.pushurl(), undefined);
  });

  it("can set a remote", function() {
    var repository = this.repository;
    Remote.create(repository, "origin1", url);

    Remote.setPushurl(repository, "origin1", "https://google.com/");

    return Remote.lookup(repository, "origin1").then(function(remote) {
      assert.equal(remote.pushurl(), "https://google.com/");
    });
  });

  it("can read the remote name", function() {
    assert.equal(this.remote.name(), "origin");
  });

  it("can create and load a new remote", function() {
    var repository = this.repository;
    Remote.create(repository, "origin2", url);

    return Remote.lookup(repository, "origin2").then(function(remote) {
      assert(remote.url(), url);
    });
  });

  it("can delete a remote", function() {
    var repository = this.repository;
    Remote.create(repository, "origin3", url);

    return Remote.delete(repository, "origin3")
      .then(function() {
        return Remote.lookup(repository, "origin3");
      })
      .then(Promise.reject, Promise.resolve);
  });

  it("can download from a remote", function() {
    var repo = this.repository;
    var remoteCallbacks;

    return repo.getRemote("origin")
      .then(function(remote) {
        remoteCallbacks = {
          certificateCheck: function() {
            return 1;
          }
        };

        return remote.connect(NodeGit.Enums.DIRECTION.FETCH, remoteCallbacks)
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

    Remote.create(repo, "test2", url2);

    return repo.getRemote("test2")
      .then(function(remote) {
        var fetchOpts = {
          callbacks: {
            credentials: function(url, userName) {
              return NodeGit.Cred.sshKeyFromAgent(userName);
            },
            certificateCheck: function() {
              return 1;
            },

            transferProgress: function() {
              wasCalled = true;
            }
          }
        };

        return remote.fetch(null, fetchOpts, null);
      })
      .then(function() {
        assert.ok(wasCalled);

        return Remote.delete(repo, "test2");
      });
  });

  it("can fetch from a remote", function() {
    return this.repository.fetch("origin", {
      callbacks: {
        credentials: function(url, userName) {
          return NodeGit.Cred.sshKeyFromAgent(userName);
        },
        certificateCheck: function() {
          return 1;
        }
      }
    });
  });

  it("can fetch from a private repository", function() {
    this.timeout(15000);

    var repo = this.repository;
    var remote = Remote.create(repo, "private", privateUrl);
    var fetchOptions = {
      callbacks: {
        credentials: function(url, userName) {
          return NodeGit.Cred.sshKeyNew(
            userName,
            path.resolve("./test/nodegit-test-rsa.pub"),
            path.resolve("./test/nodegit-test-rsa"),
            ""
          );
        },
        certificateCheck: function() {
          return 1;
        }
      }
    };

    return remote.fetch(null, fetchOptions, "Fetch from private")
      .catch(function() {
        assert.fail("Unable to fetch from private repository");
      });
  });

  it("can reject fetching from private repository without valid credentials",
    function() {
      this.timeout(15000);

      var repo = this.repository;
      var remote = Remote.create(repo, "private", privateUrl);
      var firstPass = true;
      var fetchOptions = {
        callbacks: {
          credentials: function(url, userName) {
            if (firstPass) {
              firstPass = false;
              return NodeGit.Cred.sshKeyFromAgent(userName);
            }
          },
          certificateCheck: function() {
            return 1;
          }
        }
      };

      return remote.fetch(null, fetchOptions, "Fetch from private")
        .then(function () {
          assert.fail("Should not be able to fetch from repository");
        })
        .catch(function(error) {
          assert.equal(
            error.message.trim(),
             "ERROR: Repository not found.",
             "Should not be able to find repository."
          );
        });
  });

  it("can fetch from all remotes", function() {
    // Set a reasonable timeout here for the fetchAll test
    this.timeout(15000);

    var repository = this.repository;
    Remote.create(repository, "test1", url);
    Remote.create(repository, "test2", url2);

    return repository.fetchAll({
      callbacks: {
        credentials: function(url, userName) {
          return NodeGit.Cred.sshKeyFromAgent(userName);
        },
        certificateCheck: function() {
          return 1;
        }
      }
    });
  });

  it("will reject if credentials promise rejects", function() {
    this.timeout(5000);
    var repo = this.repository;
    var branch = "should-not-exist";
    return Remote.lookup(repo, "origin")
      .then(function(remote) {
        var ref = "refs/heads/" + branch;
        var refs = [ref + ":" + ref];
        var options = {
          callbacks: {
            credentials: function(url, userName) {
              var test = Promise.resolve("test")
                .then(function() { return; })
                .then(function() { return; })
                .then(function() { return; })
                .then(function() {
                  return Promise.reject(new Error("failure case"));
                });
              return test;
            },
            certificateCheck: function() {
              return 1;
            }
          }
        };
        return remote.push(refs, options);
      })
      .then(function() {
        return Promise.reject(
          new Error("should not be able to push to the repository"));
      }, function(err) {
        if (err.message === "failure case")
        {
          return Promise.resolve();
        } else {
          throw err;
        }
      })
      .then(function() {
        return Remote.lookup(repo, "origin");
      })
      .then(function(remote) {
        var ref = "refs/heads/" + branch;
        var refs = [ref + ":" + ref];
        var options = {
          callbacks: {
            credentials: function(url, userName) {
              var test = Promise.resolve()
                .then(Promise.resolve)
                .then(Promise.resolve)
                .then(Promise.resolve)
                .then(Promise.reject);
              return test;
            },
            certificateCheck: function() {
              return 1;
            }
          }
        };
        return remote.push(refs, options);
      })
      .then(function() {
        return Promise.reject(
          new Error("should not be able to push to the repository"));
      }, function(err) {
        if (err.message === "Method push has thrown an error.")
        {
          return Promise.resolve();
        } else {
          throw err;
        }
      });
  });

  it("cannot push to a repository with invalid credentials", function() {
    this.timeout(5000);
    var repo = this.repository;
    var branch = "should-not-exist";
    return Remote.lookup(repo, "origin")
      .then(function(remote) {
        var ref = "refs/heads/" + branch;
        var refs = [ref + ":" + ref];
        var firstPass = true;
        var options = {
          callbacks: {
            credentials: function(url, userName) {
              if (firstPass) {
                firstPass = false;
                if (url.indexOf("https") === -1) {
                  return NodeGit.Cred.sshKeyFromAgent(userName);
                } else {
                  return NodeGit.Cred.userpassPlaintextNew(userName, "");
                }
              } else {
                return Promise.reject();
              }
            },
            certificateCheck: function() {
              return 1;
            }
          }
        };
        return remote.push(refs, options);
      })
      // takes care of windows bug, see the .catch for the proper pathway
      // that this flow should take (cred cb doesn't run twice -> throws error)
      .then(function() {
        return Promise.reject(
          new Error("should not be able to push to the repository"));
      }, function(err) {
        if (err.message.indexOf(401) === -1) {
          throw err;
        } else {
          return Promise.resolve();
        }
      })
      // catches linux / osx failure to use anonymous credentials
      // stops callback infinite loop
      .catch(function (reason) {
        if (reason.message !==
          "Method push has thrown an error.")
        {
          throw reason;
        } else {
          return Promise.resolve();
        }
      });
  });
});
