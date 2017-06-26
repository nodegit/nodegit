var assert = require("assert"),
  promisify = require("promisify-node"),
  fse = promisify(require("fs-extra")),
  path = require("path"),
  local = path.join.bind(path, __dirname);

describe("Filter", function() {
  var NodeGit = require("../../");
  // var Buffer = NodeGit.Buf;
  // var FilterSource = NodeGit.FilterSource;

  var emptyRepoPath = local("../repos/empty");
  var filterName = "psuedo_filter";
  var Registry = NodeGit.FilterRegistry;
  var Checkout = NodeGit.Checkout;
  var Repository = NodeGit.Repository;
    // Attr = NodeGit.Attr,
    // Status = NodeGit.Status,
  var reposPath = local("../repos/workdir");

  var packageJsonPath = path.join(reposPath, "package.json");
  var readmePath = path.join(reposPath, "README.md");
  
  function commitFile(repo, fileName, fileContent, commitMessage) {
    var index;
    var treeOid;
    var parent;

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
      var author = signatures[0];
      var committer = signatures[1];

      return repo.createCommit(
        "HEAD",
        author,
        committer,
        "message",
        treeOid,
        [parent]);
    });
  }

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      })
      .then(function() {
        return Repository.open(emptyRepoPath);
      })
      .then(function(emptyRepo) {
        test.emptyRepo = emptyRepo;
      })
      .then(function() {
        // FIXME: updated * wildcard to *.md -> update test cases
        return fse.writeFile(
          path.join(reposPath, ".gitattributes"), 
          "*.md filter="+ filterName +" diff=lfs merge=lfs -text", {
          encoding: "utf-8",
        });
      });
  });

  afterEach(function() {
    //Unregistering the filter to avoid GIT_EEXISTS
    return Registry.unregister(filterName)
      .then(function(error_code){
        if(error_code) {
          console.log("Error code: ", error_code);
        }
      })
      .catch(function(error) {
        if (error.errno !== -3) {
          throw new Error(error);
        }
      });
  });

  describe("Register and Unregister (now callback)", function(){
    it("Registering filter for the first time", function() {

      // registering custom filter
      return Registry.register(filterName, {
        apply: function() {
          // console.log("inside APPLY");
          return;
        },
        check: function(){
          // console.log("inside CHECK");
          return;
        }
      }, 0)
      .then(function(result) {
        assert.strictEqual(result, 0);
      });
    });

    it("Registering filter and re-registering same filter", function() {
      // registering custom filter
      return Registry.register(filterName, {
        apply: function() {
          return;
        },
        check: function(){
          return;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
          return;
        })
        .then(function() {
          return Registry.register(filterName, {
            apply: function() {
              return;
            },
            check: function(){
              return;
            }
          }, 0);
        })
        .catch(function(error) {
          assert.strictEqual(error.errno, -4);
        });
    });
  });

  describe("Initialize callback", function(){
    
    it("Testing initialize callback", function() {
      var test = this;
      // test if anything has changed and appy something, then check
      return Registry.register(filterName, {
        initialize: function() {
          fse.writeFileSync(
            readmePath,
            "Initialized", {
              encoding: "utf-8"
            }
          );
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
        //assert the package json does not contain Initialized TEXT
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
          paths: "package.json"
        };
        return Checkout.head(test.repository, opts);
      })
      .then(function() {
        var postInitializeReadmeContents = fse.readFileSync(
          readmePath, "utf-8");

        assert.strictEqual(
          postInitializeReadmeContents, "Initialized");
      });
    });
  });

  describe("Shutdown callback", function(){

    it("Testing shutdown callback", function(){
      var test = this;
      return Registry.register(filterName, {
        apply: function() {
          return;
        },
        check: function(){
          return -30;
        },
        shutdown: function(){
          fse.writeFileSync(
            readmePath,
            "Shutdown", {
              encoding: "utf-8"
            }
          );
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var packageContent = fse.readFileSync(packageJsonPath, "utf-8"),
          readmeContent = fse.readFileSync(readmePath, "utf-8");

          assert.notEqual(packageContent, "");
          assert.notEqual(readmeContent, "Shutdown");
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
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          return Registry.unregister(filterName);
        })
        .then(function() {
          var postUnregisterReadmeContent = fse.readFileSync(
            readmePath, "utf-8");

          assert.strictEqual(
            postUnregisterReadmeContent, "Shutdown");
        });
    });

    it("Testing shutdown callback without initialize callback", function(){
      var test = this;
      return Registry.register(filterName, {
        apply: function() {
          return;
        },
        check: function(){
          return -30;
        },
        shutdown: function(){
          fse.writeFileSync(
            readmePath,
            "Shutdown", {
              encoding: "utf-8"
            }
          );
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          var packageContent = fse.readFileSync(packageJsonPath, "utf-8"),
          readmeContent = fse.readFileSync(readmePath, "utf-8");

          assert.notEqual(packageContent, "");
          assert.notEqual(readmeContent, "Shutdown");
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
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          return Registry.unregister(filterName);
        })
        .then(function() {
          var postUnregisterReadmeContent = fse.readFileSync(
            readmePath, "utf-8");

          assert.strictEqual(
            postUnregisterReadmeContent, "Shutdown");
        });
    });
    
  });

  describe("Check and Apply callback", function(){

    it(
      "if GIT_PASSTHROUGH is returned" +
      " filter should not to be applied", function(){
      var test = this;
      
      return Registry.register(filterName, {
        apply: function() {
          fse.writeFileSync(
            readmePath,
            "Filter Applied", {
              encoding: "utf-8"
            }
          );
        },
        check: function() {
          return -30;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          //assert the package json does not contain Initialized TEXT
          var packageContent = fse.readFileSync(packageJsonPath, "utf-8"),
            readmeContent = fse.readFileSync(readmePath, "utf-8");

          assert.notStrictEqual(packageContent, "");
          assert.notStrictEqual(readmeContent, "Filter Applied");
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
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath, "utf-8");

          assert.notStrictEqual(
            postInitializeReadmeContents, "Filter Applied"
          );
        });

    });

    it("if GIT_OK is passed, filter should be applied", function() {
      var test = this;
      
      return Registry.register(filterName, {
        apply: function() {
          fse.writeFileSync(
            readmePath,
            "Filter Applied", {
              encoding: "utf-8"
            }
          );
        },
        check: function() {
          return 0;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          //assert the package json does not contain Initialized TEXT
          var packageContent = fse.readFileSync(packageJsonPath, "utf-8"),
            readmeContent = fse.readFileSync(readmePath, "utf-8");

          assert.notStrictEqual(packageContent, "");
          assert.notStrictEqual(readmeContent, "Filter Applied");
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
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath, "utf-8");

          assert.strictEqual(
            postInitializeReadmeContents, "Filter Applied"
          );
        });
    });

    it("if GIT_PASSTHROUGH is returned from Apply callback", function() {
      var test = this;
      
      var tempBuffer = new Buffer("some new fancy filter", "utf8");
      console.log("tempBuffer: ", tempBuffer);

      // check the contents of to inside of apply 
      //call back and check if the to structure is 
      //being poulated v/s just being null
      return Registry.register(filterName, {
        apply: function(to, from, source) {
          // console.log("To: ", to.ptr());
          // console.log("From: ", from.ptr());
          // to.ptr = "some random text 2";
          // console.log("To: ", to.ptr());
          // console.log("[changed]From: ", from.ptr());
          // console.log("Source: ", source);
          fse.writeFileSync(
            readmePath,
            "Filter Applied", {
              encoding: "utf-8"
            }
          );
          return 0;
        },
        check: function() {
          return 0;
        },
        cleanup: function() {
          // console.log("Inside CLEANUP");
          return;
        }
      }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          //assert the package json does not contain Initialized TEXT
          var packageContent = fse.readFileSync(packageJsonPath, "utf-8"),
            readmeContent = fse.readFileSync(readmePath, "utf-8");

          assert.notStrictEqual(packageContent, "");
          assert.notStrictEqual(readmeContent, "Filter Applied");
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
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath, "utf-8");

          assert.strictEqual(
            postInitializeReadmeContents, "Filter Applied"
          );
        });
    });

    it.only("using buffers to control data flow inside filter", function() {
      var test = this;

      return Registry.register(filterName, {
        apply: function(to, from, source) {
          var tempBuffer = new Buffer("fancy filter", "utf8");
          // console.log("tempBuffer: ", tempBuffer);

          // console.log("To: ", to.ptr());
          // console.log("From: ", from.ptr());
          var newTo = to.set(tempBuffer, 12)
            .then(function(buf) {
              // console.log("buffer: ", buf);
              return buf;
            });
          return newTo.then(function(ret) {
            console.log("To[New]: ", to.ptr());
            return 0;
          });
        },
        check: function() {
          return 0;
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
        .then(function() {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath, "utf-8");

          assert.strictEqual(
            postInitializeReadmeContents, "testing commit contents"
          );
        });
    });
  });

  describe("Cleanup callback", function() {
    it("Cleanup callback is called after" +
     " filter has been applied", function() {
       var test = this;
        // test if anything has changed and appy something, then check
        return Registry.register(filterName, {
          initialize: function() {
            fse.writeFileSync(
              readmePath,
              "Initialized", {
                encoding: "utf-8"
              }
            );
            return 0;
          },
          apply: function() {
            return 0;
          },
          check: function() {
            return 0;
          },
          cleanup: function() {
            fse.writeFileSync(
              readmePath,
              "Cleaned Up", {
                encoding: "utf-8"
              }
            );
          }
        }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          //assert the package json does not contain Initialized TEXT
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
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath, "utf-8");

          assert.strictEqual(
            postInitializeReadmeContents, "Cleaned Up");
        });
     });

     it("Cleanup callback should not be called if" +
      " Check callback returns GIT_PASSTHROUGH", function() {
       var test = this;
        // test if anything has changed and appy something, then check
        return Registry.register(filterName, {
          initialize: function() {
            fse.writeFileSync(
              readmePath,
              "Initialized", {
                encoding: "utf-8"
              }
            );
            return 0;
          },
          apply: function() {
            return 0;
          },
          check: function() {
            return -30;
          },
          cleanup: function() {
            fse.writeFileSync(
              readmePath,
              "Cleaned Up", {
                encoding: "utf-8"
              }
            );
          }
        }, 0)
        .then(function(result) {
          assert.strictEqual(result, 0);
        })
        .then(function() {
          //assert the package json does not contain Initialized TEXT
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
            paths: "package.json"
          };
          return Checkout.head(test.repository, opts);
        })
        .then(function() {
          var postInitializeReadmeContents = fse.readFileSync(
            readmePath, "utf-8");

          assert.notStrictEqual(
            postInitializeReadmeContents, "Cleaned Up");
        });
     });
  });
});
