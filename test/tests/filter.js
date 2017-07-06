var assert = require("assert"),
  promisify = require("promisify-node"),
  fse = promisify(require("fs-extra")),
  path = require("path"),
  local = path.join.bind(path, __dirname);

describe("Filter", function() {
  var NodeGit = require("../../");

  var emptyRepoPath = local("../repos/empty");
  var filterName = "psuedo_filter";
  var Registry = NodeGit.FilterRegistry;
  var Checkout = NodeGit.Checkout;
  var Repository = NodeGit.Repository;
  var reposPath = local("../repos/workdir");

  var packageJsonPath = path.join(reposPath, "package.json");
  var readmePath = path.join(reposPath, "README.md");
  
  var GIT_PASSTHROUGH = -30;

  var mockFilter = {
    apply: function() {
      return;
    },
    check: function(){
      return;
    }
  };

  function commitFile(repo, fileName, fileContent, commitMessage) {
    let index;
    let treeOid;
    let parent;

    return fse.writeFile(path.join(repo.workdir(), fileName), fileContent)
    .then(function() {
      return repo.refreshIndex();
    })
    .then(function(indexResult) {
      index = indexResult;
    })
    .then(function() {
      return index.addByPath(fileName);
    })
    .then(function() {
      return index.write();
    })
    .then(function() {
      return index.writeTree();
    })
    .then(function(oidResult) {
      treeOid = oidResult;
      return NodeGit.Reference.nameToId(repo, "HEAD");
    })
    .then(function(head) {
      return repo.getCommit(head);
    })
    .then(function(parentResult) {
      parent = parentResult;
      return Promise.all([
        NodeGit.Signature.create("Foo Bar", "foo@bar.com", 123456789, 60),
        NodeGit.Signature.create("Foo A Bar", "foo@bar.com", 987654321, 90)
      ]);
    })
    .then(function(signatures) {
      let author = signatures[0];
      let committer = signatures[1];

      return repo.createCommit(
        "HEAD",
        author,
        committer,
        commitMessage,
        treeOid,
        [parent]);
    });
  }

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
        return Repository.open(emptyRepoPath);
      })
      .then(function(emptyRepo) {
        test.emptyRepo = emptyRepo;
        return fse.writeFile(
          path.join(reposPath, ".gitattributes"), 
          "*.md filter="+ filterName +" -text", {
          encoding: "utf-8",
        });
      });
  });

  afterEach(function() {
    return Registry.unregister(filterName)
      .catch(function(error) {
        switch(error) {
          case -1:
            throw new Error("Cannot unregister filter");
          // case -3:
          //   throw new Error('Cannot find filter to unregister');
          default:
            return;
        }
      });
  });

  describe("Register", function() {
    var secondFilter = "hellofilter";

    after(function(done) {
      return Registry.unregister(secondFilter)
        .then(function() {
          done();
        });
    });

    it("can register a filter", function() {
      return Registry.register(filterName, mockFilter, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        });
    });

    it("can register multiple filters", function() {
      return Registry.register(filterName, mockFilter, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
          return Registry.register(secondFilter, mockFilter, 1);
        })
        .then(function(result) {
          assert.strictEqual(result, 0);
        });
    });

    it("cannot register the same filter twice", function() {
      return Registry.register(filterName, mockFilter, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
          return Registry.register(filterName, mockFilter, 0);
        })
        .catch(function(error) {
          assert.strictEqual(error.errno, -4);
        });
    });
  });

  describe("Unregister", function() {
    beforeEach(function() {
      return Registry.register(filterName, mockFilter, 0);
    });

    it("can unregister the filter", function() {
      return Registry.unregister(filterName)
        .then(function(result) {
          assert.strictEqual(result, 0);
        });
    });

    it("cannot unregister the filter twice", function() {
      return Registry.unregister(filterName)
        .then(function(result) { 
          assert.strictEqual(result, 0);
          return Registry.unregister(filterName);
        })
        .then(function(result) {
          assert.fail(result, -3, "Should not have unregistered successfully");
        })
        .catch(function(error) {
          assert.strictEqual(error.errno, -3);
        });
    });
  });

  describe("Initialize", function(){
    it("initializes successfully", function() {
      var test = this;
      var initialized = false;
      return Registry.register(filterName, {
        initialize: function() {
          initialized = true;
          return 0;
        },
        apply: function() {
          return;
        },
        check: function() {
          return -30;
        }
      }, 0)
      .then(function(result) {
        assert.strictEqual(result, 0);
      })
      .then(function() {
        return fse.writeFile(
          packageJsonPath, 
          "Changing content to trigger checkout");
      })
      .then(function() {
        var opts = {
          checkoutStrategy: Checkout.STRATEGY.FORCE,
          paths: "package.json"
        };
        return Checkout.head(test.repository, opts);
      })
      .then(function() {
        assert.strictEqual(initialized, true);
      });
    });

    it("initializes successfully even on garbage collect", function() {
      var test = this;
      var initialized = false;
      return Registry.register(filterName, {
        initialize: function() {
          initialized = true;
          return 0;
        },
        apply: function() {
          return;
        },
        check: function() {
          return -30;
        }
      }, 0)
      .then(function(result) {
        assert.strictEqual(result, 0);
        global.gc();

        return fse.writeFile(
          packageJsonPath, 
          "Changing content to trigger checkout");
      })
      .then(function() {
        var opts = {
          checkoutStrategy: Checkout.STRATEGY.FORCE,
          paths: "package.json"
        };
        return Checkout.head(test.repository, opts);
      })
      .then(function() {
        assert.strictEqual(initialized, true);
      });
    });

    it("does not initialize successfully", function() {
      var test = this;
      var initialized = false;
      return Registry.register(filterName, {
        initialize: function() {
          initialized = true;
          return -1;
        },
        apply: function() {
          return;
        },
        check: function() {
          return -30;
        }
      }, 0)
      .then(function(result) {
        assert.strictEqual(result, 0);
      })
      .then(function() {
        return fse.writeFile(
          packageJsonPath, 
          "Changing content to trigger checkout");
      })
      .then(function() {
        var opts = {
          checkoutStrategy: Checkout.STRATEGY.FORCE,
          paths: "package.json"
        };
        return Checkout.head(test.repository, opts);
      })
      .then(function(head) {
        assert.fail(head, undefined, "Should not have actually checked out");
      })
      .catch(function(error) {
        assert.strictEqual(initialized, true);
      });
    });
  });

  describe("Shutdown", function() {
    it("filter successfully shuts down", function() {
      var test = this;
      var shutdown = false;
      return Registry.register(filterName, {
        apply: function() {
          return;
        },
        check: function(){
          return -30;
        },
        shutdown: function(){
          shutdown = true;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
          return fse.writeFile(
            packageJsonPath, 
            "Changing content to trigger checkout", {
            encoding: "utf-8",
          });
        })
        .then(function() {
          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          return Registry.unregister(filterName);
        })
        .then(function(result) {
          assert.strictEqual(result, 0);
          assert.strictEqual(shutdown, true);
        });
    });
    
    it("filter successfully shuts down on garbage collect", function() {
      var test = this;
      var shutdown = false;
      return Registry.register(filterName, {
        apply: function() {
          return;
        },
        check: function(){
          return -30;
        },
        shutdown: function(){
          shutdown = true;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
          return fse.writeFile(
            packageJsonPath, 
            "Changing content to trigger checkout", {
            encoding: "utf-8",
          });
        })
        .then(function() {
          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          global.gc();
          return Registry.unregister(filterName);
        })
        .then(function(result) {
          assert.strictEqual(result, 0);
          assert.strictEqual(shutdown, true);
        });
    });

    it("shutdown completes even if there is an error", function() {
      var test = this;
      var shutdown = false;
      return Registry.register(filterName, {
        apply: function() {
          return;
        },
        check: function(){
          return -30;
        },
        shutdown: function(){
          shutdown = true;
          throw new Error("I failed");
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
          return fse.writeFile(
            packageJsonPath, 
            "Changing content to trigger checkout", {
            encoding: "utf-8",
          });
        })
        .then(function() {
          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          return Registry.unregister(filterName);
        })
        .then(function(result) {
          assert.strictEqual(result, 0);
          assert.strictEqual(shutdown, true);
        })
        .catch(function(error) {
          assert.fail(error, null, "The operation should not have failed");
        });
    });
  });

  describe("Apply", function() {
    var message = "some new fancy filter";
    var length = message.length;
    var tempBuffer = new Buffer(message, "utf-8");

    it("should not apply when check returns GIT_PASSTHROUGH", function(){
      var test = this;
      var applied = false;
      
      return Registry.register(filterName, {
        apply: function() {
          applied = true;
        },
        check: function() {
          return GIT_PASSTHROUGH;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
          return fse.writeFile(
            packageJsonPath, 
            "Changing content to trigger checkout", {
            encoding: "utf-8",
          });
        })
        .then(function() {
          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          assert.notStrictEqual(applied, true);
        });
    });

    it("should apply filter when check succeeds", function() {
      var test = this;
      var applied = true;

      return Registry.register(filterName, {
        apply: function() {
          applied = true;
        },
        check: function() {
          return 0;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
          return fse.writeFile(
            packageJsonPath, 
            "Changing content to trigger checkout", {
            encoding: "utf-8",
          });
        })
        .then(function() {
          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          assert.strictEqual(applied, true);
        });
    });

    it("does not apply when GIT_PASSTHROUGH is returned", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          return to.set(tempBuffer, length)
            .then(function() {
              return GIT_PASSTHROUGH;
            });
        },
        check: function() {
          return 0;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(packageJsonPath, "utf-8");
          assert.notStrictEqual(readmeContent, message);

          return fse.writeFile(
            packageJsonPath, 
            "Changing content to trigger checkout");
        })
        .then(function() {
          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath, "utf-8");

          assert.notStrictEqual(postInitializeReadmeContents, message);
        });
    });

    it("applies the filter data on checkout", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          return to.set(tempBuffer, length)
            .then(function(buf) {
              return 0;
            });
        },
        check: function(src, attr) {
          return 0;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(readmePath, "utf-8");
          assert.notStrictEqual(readmeContent, message);
          fse.writeFileSync(readmePath, "whoa", "utf8");

          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: ["README.md"]
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath, "utf-8");

          assert.strictEqual(postInitializeReadmeContents, message);
        });
    });

    it("applies the filter data on checkout with gc", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          return to.set(tempBuffer, length)
            .then(function(buf) {
              return 0;
            });
        },
        check: function(src, attr) {
          return 0;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(readmePath, "utf-8");
          assert.notStrictEqual(readmeContent, message);
          fse.writeFileSync(readmePath, "whoa", "utf8");
          global.gc();

          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: ["README.md"]
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath, "utf-8");

          assert.strictEqual(postInitializeReadmeContents, message);
        });
    });

    it("applies the filter data on commit", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          return to.set(tempBuffer, length)
            .then(function(buf) {
              return 0;
            });
        },
        check: function(src, attr) {
          return src.path() === "README.md" ? 0 : GIT_PASSTHROUGH;
        },
        cleanup: function() {
          return;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(readmePath, "utf-8");
          assert.notStrictEqual(readmeContent, "testing commit contents");
        })
        .then(function() {
          return commitFile(test.repository, "README.md", 
            "testing commit contents", 
            "test commit");
        })
        .then(function(oid) {
          return test.repository.getHeadCommit();
        })
        .then(function(commit) {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath, "utf-8");

          assert.strictEqual(
            postInitializeReadmeContents, "testing commit contents"
          );
          assert.strictEqual(commit.message(), "test commit");

          return commit.getEntry("README.md");
        })
        .then(function(entry) {
          assert.strictEqual(entry.isBlob(), true);
          return entry.getBlob();
        })
        .then(function(blob) {
          assert.strictEqual(blob.toString(), message);
        });
    });

    it("applies the filter data on commit with gc", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          return to.set(tempBuffer, length)
            .then(function(buf) {
              return 0;
            });
        },
        check: function(src, attr) {
          return src.path() === "README.md" ? 0 : GIT_PASSTHROUGH;
        },
        cleanup: function() {
          return;
        }
      }, 0)
        .then(function(result) {
          global.gc();
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(readmePath, "utf-8");
          assert.notStrictEqual(readmeContent, "testing commit contents");
        })
        .then(function() {
          return commitFile(test.repository, "README.md", 
            "testing commit contents", 
            "test commit");
        })
        .then(function(oid) {
          global.gc();
          return test.repository.getHeadCommit();
        })
        .then(function(commit) {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath, "utf-8");

          assert.strictEqual(
            postInitializeReadmeContents, "testing commit contents"
          );
          assert.strictEqual(commit.message(), "test commit");
          global.gc();

          return commit.getEntry("README.md");
        })
        .then(function(entry) {
          assert.strictEqual(entry.isBlob(), true);
          return entry.getBlob();
        })
        .then(function(blob) {
          assert.strictEqual(blob.toString(), message);
        });
    });
  });

  describe("Cleanup", function() {
    it("is called successfully", function() {
      var test = this;
      var cleaned = false;
      return Registry.register(filterName, {
        initialize: function() {
          return 0;
        },
        apply: function() {
          return 0;
        },
        check: function() {
          return 0;
        },
        cleanup: function() {
          cleaned = true;
        }
      }, 0)
      .then(function(result) {
        assert.strictEqual(result, 0);
      })
      .then(function() {
        var packageContent = fse.readFileSync(packageJsonPath, "utf-8");
        assert.notEqual(packageContent, "");

        return fse.writeFile(
          packageJsonPath, 
          "Changing content to trigger checkout", {
          encoding: "utf-8",
        });
      })
      .then(function() {
        var opts = {
          checkoutStrategy: Checkout.STRATEGY.FORCE,
          paths: "package.json"
        };
        return Checkout.head(test.repository, opts);
      })
      .then(function() {
        assert.strictEqual(cleaned, true);
      });
    });

    it("is called successfully with gc", function() {
      var test = this;
      var cleaned = false;
      return Registry.register(filterName, {
        initialize: function() {
          return 0;
        },
        apply: function() {
          return 0;
        },
        check: function() {
          return 0;
        },
        cleanup: function() {
          cleaned = true;
        }
      }, 0)
      .then(function(result) {
        assert.strictEqual(result, 0);
      })
      .then(function() {
        var packageContent = fse.readFileSync(packageJsonPath, "utf-8");
        assert.notEqual(packageContent, "");

        global.gc();
        return fse.writeFile(
          packageJsonPath, 
          "Changing content to trigger checkout", {
          encoding: "utf-8",
        });
      })
      .then(function() {
        var opts = {
          checkoutStrategy: Checkout.STRATEGY.FORCE,
          paths: "package.json"
        };
        return Checkout.head(test.repository, opts);
      })
      .then(function() {
        assert.strictEqual(cleaned, true);
      });
    });

    it("is not called when apply returns GIT_PASSTHROUGH", function() {
      var test = this;
      var cleaned = false;

      return Registry.register(filterName, {
        initialize: function() {
          return 0;
        },
        apply: function() {
          return GIT_PASSTHROUGH;
        },
        check: function() {
          return 0;
        },
        cleanup: function() {
          cleaned = true;
        }
      }, 0)
      .then(function(result) {
        assert.strictEqual(result, 0);
      })
      .then(function() {
        var packageContent = fse.readFileSync(packageJsonPath, "utf-8"),
          readmeContent = fse.readFileSync(readmePath, "utf-8");

        assert.notEqual(packageContent, "");
        assert.notEqual(readmeContent, "Initialized");
      })
      .then(function() {
        return fse.writeFile(
          packageJsonPath, 
          "Changing content to trigger checkout", {
          encoding: "utf-8",
        });
      })
      .then(function() {
        var opts = {
          checkoutStrategy: Checkout.STRATEGY.FORCE,
          paths: "README.md"
        };
        return Checkout.head(test.repository, opts);
      })
      .then(function() {
        assert.notStrictEqual(cleaned, true);
      });
    });
  });
});
