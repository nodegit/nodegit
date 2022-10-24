var assert = require("assert");
var fse = require("fs-extra");
var path = require("path");
var local = path.join.bind(path, __dirname);
var garbageCollect = require("../utils/garbage_collect.js");

describe("Filter", function() {
  var NodeGit = require("../../");

  var emptyRepoPath = local("../repos/empty");
  var filterName = "psuedo_filter";
  var Registry = NodeGit.FilterRegistry;
  var Checkout = NodeGit.Checkout;
  var Repository = NodeGit.Repository;
  var FilterList = NodeGit.FilterList;
  var reposPath = local("../repos/workdir");

  var packageJsonPath = path.join(reposPath, "package.json");
  var readmePath = path.join(reposPath, "README.md");

  var mockFilter = {
    apply: function() {},
    check: function() {}
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
          [parent]
        );
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
          "*.md filter=" + filterName + " -text",
          { encoding: "utf-8" }
        );
      });
  });

  afterEach(function() {
    return Registry.unregister(filterName)
      .catch(function(error) {
        if (error === NodeGit.Error.CODE.ERROR) {
          throw new Error("Cannot unregister filter");
        }
      });
  });

  describe("Register", function() {
    var secondFilter = "hellofilter";

    after(function(done) {
      Registry.unregister(secondFilter)
        .then(function() {
          done();
        });
    });

    it("can register a filter", function() {
      return Registry.register(filterName, mockFilter, 0)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
        });
    });

    it("can register multiple filters", function() {
      return Registry.register(filterName, mockFilter, 0)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
          return Registry.register(secondFilter, mockFilter, 1);
        })
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
        });
    });

    it("cannot register the same filter twice", function() {
      return Registry.register(filterName, mockFilter, 0)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
          return Registry.register(filterName, mockFilter, 0);
        })
        .catch(function(error) {
          assert.strictEqual(error.errno, NodeGit.Error.CODE.EEXISTS);
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
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
        });
    });

    it("cannot unregister the filter twice", function() {
      return Registry.unregister(filterName)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
          return Registry.unregister(filterName);
        })
        .then(function(result) {
          assert.fail("Should not have unregistered successfully");
        })
        .catch(function(error) {
          assert.strictEqual(error.errno, NodeGit.Error.CODE.ENOTFOUND);
        });
    });
  });

  describe("Apply", function() {
    before(function() {
      var test = this;
      return fse.readFile(readmePath, "utf8")
        .then((function(content) {
          test.originalReadmeContent = content;
        }));
    });

    afterEach(function() {
      this.timeout(15000);
      return fse.writeFile(readmePath, this.originalReadmeContent);
    });

    var message = "some new fancy filter";
    var length = message.length;
    var tempBuffer = Buffer.from(message, "utf-8");
    var largeBufferSize = 500000000;

    it("should not apply when check returns GIT_PASSTHROUGH", function(){
      var test = this;
      var applied = false;

      return Registry.register(filterName, {
        apply: function() {
          applied = true;
        },
        check: function() {
          return NodeGit.Error.CODE.PASSTHROUGH;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
          return fse.writeFile(
            packageJsonPath,
            "Changing content to trigger checkout",
            { encoding: "utf-8" }
          );
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
      var applied = false;

      return Registry.register(filterName, {
        apply: function() {
          applied = true;
        },
        check: function() {
          return NodeGit.Error.CODE.OK;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
          return fse.writeFile(
            packageJsonPath,
            "Changing content to trigger checkout",
            { encoding: "utf-8" }
          );
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
          to.set(tempBuffer, length);
          return NodeGit.Error.CODE.PASSTHROUGH;
        },
        check: function() {
          return NodeGit.Error.CODE.OK;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(
            packageJsonPath,
            "utf-8"
          );
          assert.notStrictEqual(readmeContent, message);

          return fse.writeFile(
            packageJsonPath,
            "Changing content to trigger checkout"
          );
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
            readmePath,
            "utf-8"
          );

          assert.notStrictEqual(postInitializeReadmeContents, message);
        });
    });

    it("applies the filter data on checkout", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          to.set(tempBuffer, length);
          return NodeGit.Error.CODE.OK;
        },
        check: function(src, attr) {
          return NodeGit.Error.CODE.OK;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(
            readmePath,
            "utf-8"
          );
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
            readmePath,
            "utf-8"
          );

          assert.strictEqual(postInitializeReadmeContents, message);
        });
    });

    it("can run sync callback on checkout without deadlocking", function() { // eslint-disable-line
      var test = this;
      var syncCallbackResult = 1;

      return Registry.register(filterName, {
        apply: function() {
          syncCallbackResult = test.repository.isEmpty();
        },
        check: function() {
          return NodeGit.Error.CODE.OK;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
          return fse.writeFile(
            packageJsonPath,
            "Changing content to trigger checkout",
            { encoding: "utf-8" }
          );
        })
        .then(function() {
          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          assert.strictEqual(syncCallbackResult, 0);
        });
    });

    // Temporary workaround for LFS checkout. Test skipped.
    // To activate when reverting workaround.
    // 'Checkout.head' and 'Submodule.lookup' do work with the repo locked.
    // They should work together without deadlocking.
    it.skip("can run async callback on checkout without deadlocking", function() { // eslint-disable-line
      var test = this;
      var submoduleNameIn = "vendor/libgit2";
      var asyncCallbackResult = "";

      return Registry.register(filterName, {
        apply: function() {
          return NodeGit.Submodule.lookup(test.repository, submoduleNameIn)
            .then(function(submodule) {
              return submodule.name();
            })
            .then(function(name) {
              asyncCallbackResult = name;
              return NodeGit.Error.CODE.OK;
            });
        },
        check: function() {
          return NodeGit.Error.CODE.OK;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
          return fse.writeFile(
            packageJsonPath,
            "Changing content to trigger checkout",
            { encoding: "utf-8" }
          );
        })
        .then(function() {
          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          assert.equal(asyncCallbackResult, submoduleNameIn);
        });
    });

    // this test is useless on 32 bit CI, because we cannot construct
    // a buffer big enough to test anything of significance :)...
    if (process.arch === "x64") {
      it("applies the massive filter data on checkout", function() {
        this.timeout(350000);
        var test = this;
        var largeBuffer = Buffer.alloc(largeBufferSize, "a");

        return Registry.register(filterName, {
          apply: function(to, from, source) {
            to.set(largeBuffer, largeBufferSize);
            return NodeGit.Error.CODE.OK;
          },
          check: function(src, attr) {
            return NodeGit.Error.CODE.OK;
          }
        }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var fd = fse.openSync(readmePath, "r");
          var readBuf = Buffer.allocUnsafe(largeBufferSize);
          var readLength = fse.readSync(
            fd,
            readBuf,
            0,
            largeBufferSize,
            0
          );
          fse.closeSync(fd);

          assert.notStrictEqual(readLength, largeBufferSize);
          fse.writeFileSync(readmePath, "whoa", "utf8");

          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: ["README.md"]
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          var fd = fse.openSync(readmePath, "r");
          var readBuf = Buffer.allocUnsafe(largeBufferSize);
          var readLength = fse.readSync(
            fd,
            readBuf,
            0,
            largeBufferSize,
            0
          );
          fse.closeSync(fd);

          assert.strictEqual(
            readLength,
            largeBufferSize
          );
        });
      });
    }

    it("applies the filter data on checkout with gc", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          to.set(tempBuffer, length);
          return NodeGit.Error.CODE.OK;
        },
        check: function(src, attr) {
          return NodeGit.Error.CODE.OK;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(
            readmePath,
            "utf-8"
          );
          assert.notStrictEqual(readmeContent, message);
          fse.writeFileSync(readmePath, "whoa", "utf8");
          garbageCollect();

          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: ["README.md"]
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath,
            "utf-8"
          );

          assert.strictEqual(postInitializeReadmeContents, message);
        });
    });

    it("applies the filter data on commit", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          to.set(tempBuffer, length);
          return NodeGit.Error.CODE.OK;
        },
        check: function(src, attr) {
          return src.path() === "README.md" ?
            0 : NodeGit.Error.CODE.PASSTHROUGH;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(
            readmePath,
            "utf-8"
          );
          assert.notStrictEqual(readmeContent, "testing commit contents");
        })
        .then(function() {
          return commitFile(test.repository, "README.md",
            "testing commit contents",
            "test commit"
          );
        })
        .then(function(oid) {
          return test.repository.getHeadCommit();
        })
        .then(function(commit) {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath,
            "utf-8"
          );

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
          to.set(tempBuffer, length);
          return NodeGit.Error.CODE.OK;
        },
        check: function(src, attr) {
          return src.path() === "README.md" ?
            0 : NodeGit.Error.CODE.PASSTHROUGH;
        }
      }, 0)
        .then(function(result) {
          garbageCollect();
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(
            readmePath,
            "utf-8"
          );
          assert.notStrictEqual(readmeContent, "testing commit contents");
        })
        .then(function() {
          return commitFile(test.repository, "README.md",
            "testing commit contents",
            "test commit"
          );
        })
        .then(function(oid) {
          garbageCollect();
          return test.repository.getHeadCommit();
        })
        .then(function(commit) {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath,
            "utf-8"
          );

          assert.strictEqual(
            postInitializeReadmeContents, "testing commit contents"
          );
          assert.strictEqual(commit.message(), "test commit");
          garbageCollect();

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

  describe("Manually Apply", function() {
    beforeEach(function() {
      var test = this;
      var opts = {
        checkoutStrategy: Checkout.STRATEGY.FORCE,
        paths: "README.md"
      };
      return Checkout.head(test.repository, opts)
        .then(() => fse.readFile(readmePath, "utf8"))
        .then((function(content) {
          test.originalReadmeContent = content;
        }));
    });

    afterEach(function() {
      this.timeout(15000);
      return fse.writeFile(readmePath, this.originalReadmeContent);
    });

    var message = "This is the filtered content, friends";
    var length = message.length;
    var tempBuffer = Buffer.from(message, "utf-8");

    it("applies the filters for a path on demand", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          to.set(tempBuffer, length);
          return NodeGit.Error.CODE.OK;
        },
        check: function(src, attr) {
          return NodeGit.Error.CODE.OK;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(
            readmePath,
            "utf-8"
          );
          assert.notStrictEqual(readmeContent, message);
          fse.writeFileSync(readmePath, "whoa", "utf8");

          return FilterList.load(
            test.repository,
            null,
            "README.md",
            NodeGit.Filter.MODE.CLEAN,
            NodeGit.Filter.FLAG.DEFAULT
          );
        })
        .then(function(list) {
          return list.applyToFile(test.repository, "README.md");
        })
        .then(function(content) {
          assert.equal(content, message);
        });
    });

    it("applies the filters to a buffer on demand", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          to.set(tempBuffer, length);
          return NodeGit.Error.CODE.OK;
        },
        check: function(src, attr) {
          return NodeGit.Error.CODE.OK;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(
            readmePath,
            "utf-8"
          );
          assert.notStrictEqual(readmeContent, message);
          fse.writeFileSync(readmePath, "whoa", "utf8");

          return FilterList.load(
            test.repository,
            null,
            "README.md",
            NodeGit.Filter.MODE.CLEAN,
            NodeGit.Filter.FLAG.DEFAULT
          );
        })
        .then(function(list) {
          /* eslint-disable */
          return list.applyToData(new String("garbo garbo garbo garbo"));
          /* eslint-enable */
        })
        .then(function(content) {
          assert.equal(content, message);
        });
    });

    it("applies the filters to a blob on demand", function() {
      var test = this;
      var list;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          to.set(tempBuffer, length);
          return NodeGit.Error.CODE.OK;
        },
        check: function(src, attr) {
          return NodeGit.Error.CODE.OK;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(
            readmePath,
            "utf-8"
          );
          assert.notStrictEqual(readmeContent, message);
          fse.writeFileSync(readmePath, "whoa", "utf8");

          return FilterList.load(
            test.repository,
            null,
            "README.md",
            NodeGit.Filter.MODE.CLEAN,
            NodeGit.Filter.FLAG.DEFAULT
          );
        })
        .then(function(_list) {
          list = _list;

          return test.repository.getHeadCommit();
        })
        .then(function(commit) {
          return commit.getTree();
        })
        .then(function(tree) {
          return tree.entryByPath("README.md");
        })
        .then(function(entry) {
          return test.repository.getBlob(entry.id());
        })
        .then(function(blob) {
          return list.applyToBlob(blob);
        })
        .then(function(content) {
          assert.equal(content, message);
          list = null;
        });
    });
  });

  describe("FilterSource", function() {
    var message = "some new fancy filter";

    before(function() {
      var test = this;
      return fse.readFile(readmePath, "utf8")
        .then((function(content) {
          test.originalReadmeContent = content;
        }));
    });

    afterEach(function() {
      this.timeout(15000);
      return fse.writeFile(readmePath, this.originalReadmeContent);
    });

    it("a FilterSource has an async repo getter", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          return source.repo()
            .then(function() {
              return NodeGit.Error.CODE.PASSTHROUGH;
            });
        },
        check: function(source) {
          return source.repo()
            .then(function() {
              return NodeGit.Error.CODE.OK;
            });
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, NodeGit.Error.CODE.OK);
        })
        .then(function() {
          var readmeContent = fse.readFileSync(
            packageJsonPath,
            "utf-8"
          );
          assert.notStrictEqual(readmeContent, message);

          return fse.writeFile(
            packageJsonPath,
            "Changing content to trigger checkout"
          );
        })
        .then(function() {
          var opts = {
            checkoutStrategy: Checkout.STRATEGY.FORCE,
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          garbageCollect();
        });
    });
  });
});
