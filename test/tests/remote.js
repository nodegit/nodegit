var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var _ = require("lodash");
var fp = require("lodash/fp");

var garbageCollect = require("../utils/garbage_collect.js");

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

    return Remote.create(repository, "origin1", url)
      .then(function() {
        return Remote.setPushurl(repository, "origin1", "https://google.com/");
      })
      .then(function() {
        return Remote.lookup(repository, "origin1");
      })
      .then(function(remote) {
        assert.equal(remote.pushurl(), "https://google.com/");
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
      .then(function() {
        return Remote.delete(repository, "origin3");
      })
      .then(function() {
        return Remote.lookup(repository, "origin3")
          // We only want to catch the failed lookup
          .then(Promise.reject.bind(Promise), Promise.resolve.bind(Promise));
      });
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

    return Remote.create(repo, "test2", url2)
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

  it("can get the default branch of a remote", function() {
    var remoteCallbacks = {
      certificateCheck: function() {
        return 1;
      }
    };

    var remote = this.remote;

    return remote.connect(NodeGit.Enums.DIRECTION.FETCH, remoteCallbacks)
      .then(function() { return remote.defaultBranch(); })
      .then(function(branchName) {
        assert.equal("refs/heads/master", branchName);
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
    var repo = this.repository;
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

    return Remote.create(repo, "private", privateUrl)
      .then(function(remote) {
        return remote.fetch(null, fetchOptions, "Fetch from private");
      })
      .catch(function() {
        assert.fail("Unable to fetch from private repository");
      });
  });

  it("can reject fetching from private repository without valid credentials",
    function() {
      var repo = this.repository;
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

      return Remote.create(repo, "private", privateUrl)
        .then(function(remote) {
          return remote.fetch(null, fetchOptions, "Fetch from private");
        })
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
    var repository = this.repository;

    return Remote.create(repository, "test1", url)
      .then(function() {
        return Remote.create(repository, "test2", url2);
      })
      .then(function() {
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
  });

  it("will reject if credentials promise rejects", function() {
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
                .then(Promise.resolve.bind(Promise))
                .then(Promise.resolve.bind(Promise))
                .then(Promise.resolve.bind(Promise))
                .then(Promise.reject.bind(Promise));
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
        const messageWithoutNewlines = reason.message.replace(/\n|\r/g, "");
        const validErrors = [
          "Method push has thrown an error.",
          "failed to set credentials: The parameter is incorrect."
        ];
        assert.ok(
          _.includes(validErrors, messageWithoutNewlines),
          "Unexpected error: " + reason.message
        );
      });
  });

  it("is kept alive by refspec", function() {
    var repo = this.repository;
    var Remote = NodeGit.Remote;

    garbageCollect();
    var startSelfFreeingCount = Remote.getSelfFreeingInstanceCount();
    var startNonSelfFreeingCount = Remote.getNonSelfFreeingConstructedCount();

    var resolve;
    var promise = new Promise(function(_resolve) { resolve = _resolve; });

    var remote;

    repo.getRemote("origin")
      .then(function(_remote) {
        remote = _remote;
        setTimeout(resolve, 0);
      });

    return promise
      .then(function() {
        // make sure we have created one self-freeing remote
        assert.equal(startSelfFreeingCount + 1,
          Remote.getSelfFreeingInstanceCount());
        assert.equal(startNonSelfFreeingCount,
          Remote.getNonSelfFreeingConstructedCount());
        var refspec = remote.getRefspec(0);
        assert.equal("refs/heads/*", refspec.src());
        remote = null;
        garbageCollect();
        // the refspec should be holding on to the remote
        assert.equal(startSelfFreeingCount + 1,
          Remote.getSelfFreeingInstanceCount());

        assert.equal("refs/heads/*", refspec.src());

        refspec = null;
        garbageCollect();
        // the remote should be freed now
        assert.equal(startSelfFreeingCount,
          Remote.getSelfFreeingInstanceCount());
      });
  });

  it("can retrieve the list of references advertised by a remote", function() {
    var expectedRemoteHeads = {
      HEAD: {
        local: 0,
        oid: "32789a79e71fbc9e04d3eff7425e1771eb595150",
        loid: "0000000000000000000000000000000000000000",
        name: "HEAD",
        symrefTarget: "refs/heads/master"
      },
      "refs/heads/checkout-test": {
        local: 0,
        oid: "1729c73906bb8467f4095c2f4044083016b4dfde",
        loid: "0000000000000000000000000000000000000000",
        name: "refs/heads/checkout-test",
        symrefTarget: null
      },
      "refs/heads/master": {
        local: 0,
        oid: "32789a79e71fbc9e04d3eff7425e1771eb595150",
        loid: "0000000000000000000000000000000000000000",
        name: "refs/heads/master",
        symrefTarget: null
      },
      "refs/heads/rev-walk": {
        local: 0,
        oid: "32789a79e71fbc9e04d3eff7425e1771eb595150",
        loid: "0000000000000000000000000000000000000000",
        name: "refs/heads/rev-walk",
        symrefTarget: null
      },
      "refs/tags/annotated-tag": {
        local: 0,
        oid: "dc800017566123ff3c746b37284a24a66546667e",
        loid: "0000000000000000000000000000000000000000",
        name: "refs/tags/annotated-tag",
        symrefTarget: null
      },
      "refs/tags/annotated-tag^{}": {
        local: 0,
        oid: "32789a79e71fbc9e04d3eff7425e1771eb595150",
        loid: "0000000000000000000000000000000000000000",
        name: "refs/tags/annotated-tag^{}",
        symrefTarget: null
      },
      "refs/tags/light-weight-tag": {
        local: 0,
        oid: "32789a79e71fbc9e04d3eff7425e1771eb595150",
        loid: "0000000000000000000000000000000000000000",
        name: "refs/tags/light-weight-tag",
        symrefTarget: null
      }
    };

    return this.repository.getRemote("origin")
      .then(function(remote) {
        return Promise.all([
          remote,
          remote.connect(NodeGit.Enums.DIRECTION.FETCH)
        ]);
      })
      .then(function(results) {
        var remote = results[0];
        return Promise.all([remote, remote.referenceList()]);
      })
      .then(function(results) {
        var remote = results[0];
        var remoteHeads = results[1];
        var remoteHeadsBySha = fp.flow([
          fp.map(function(remoteHead) {
            return {
              local: remoteHead.local(),
              oid: remoteHead.oid().toString(),
              loid: remoteHead.loid().toString(),
              name: remoteHead.name(),
              symrefTarget: remoteHead.symrefTarget()
            };
          }),
          fp.keyBy("name")
        ])(remoteHeads);

        fp.flow([
          fp.keys,
          fp.forEach(function(remoteHeadName) {
            assert(fp.isEqual(
              expectedRemoteHeads[remoteHeadName],
              remoteHeadsBySha[remoteHeadName]
            ), "Expectations for head " + remoteHeadName + " were not met.");
          })
        ])(expectedRemoteHeads);

        return remote.disconnect();
      });
  });

  it("will error when retrieving reference list if not connected", function() {
    return this.repository.getRemote("origin")
      .then(function(remote) {
        return remote.referenceList();
      })
      .then(function() {
        assert.fail("Unconnected remote should have no reference list.");
      })
      .catch(function(notConnectedError) {
        assert(notConnectedError.message === "this remote has never connected");
      });
  });
});
