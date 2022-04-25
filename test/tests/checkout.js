var assert = require("assert");
var path = require("path");
var fse = require("fs-extra");
var local = path.join.bind(path, __dirname);

describe("Checkout", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Checkout = NodeGit.Checkout;

  var readMeName = "README.md";
  var packageJsonName = "package.json";
  var reposPath = local("../repos/workdir");
  var readMePath = local("../repos/workdir/" + readMeName);
  var packageJsonPath = local("../repos/workdir/" + packageJsonName);
  var checkoutBranchName = "checkout-test";
  var longpathBranchName = "longpaths-checkout";

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repo) {
        test.repository = repo;
      });
  });

  it("can checkout the head", function() {
    var test = this;

    return Checkout.head(test.repository)
    .then(function(blob) {
      var packageContent = fse.readFileSync(packageJsonPath, "utf-8");

      assert.ok(~packageContent.indexOf("\"ejs\": \"~1.0.0\","));
    });
  });

  it("can checkout a branch with a long file path", function() {
    var test = this;

    return (function () {
      if(process.platform === "win32") {
        return test.repository.config()
        .then(function(config) {
          return config.setBool("core.longpaths", true);
        });
      }

      return Promise.resolve();
    })()
      .then(function() {
        return test.repository.checkoutBranch(longpathBranchName);
      })
      .then(function() {
        return test.repository.getStatus();
      })
      .then(function(statuses) {
        assert.equal(statuses.length, 0);
      });
  });

  it("cannot checkout long path file if core.longpaths is not set on win32", function() {
    var test = this;

    if (process.platform !== "win32") {
      this.skip();
    }

    return test.repository.config()
      .then(function(config) {
        config.setBool("core.longpaths", false);
      })
      .then(function () {
        return test.repository.checkoutBranch(longpathBranchName);
      })
      .then(function() {
        assert.fail();
      })
      .catch(function(err) {
        assert(~err.message.indexOf("path too long"));
      });
  });

  it("can force checkout a single file", function() {
    var test = this;

    var packageContent = fse.readFileSync(packageJsonPath, "utf-8");
    var readmeContent = fse.readFileSync(readMePath, "utf-8");

    assert.notEqual(packageContent, "");
    assert.notEqual(readmeContent, "");

    fse.outputFileSync(readMePath, "");
    fse.outputFileSync(packageJsonPath, "");

    var opts = {
      checkoutStrategy: Checkout.STRATEGY.FORCE,
      paths: packageJsonName
    };

    return Checkout.head(test.repository, opts)
    .then(function() {
      var resetPackageContent = fse.readFileSync(packageJsonPath, "utf-8");
      var resetReadmeContent = fse.readFileSync(readMePath, "utf-8");

      assert.equal(resetPackageContent, packageContent);
      assert.equal(resetReadmeContent, "");

      var resetOpts = {
        checkoutStrategy: Checkout.STRATEGY.FORCE
      };

      return Checkout.head(test.repository, resetOpts);
    }).then(function() {
      var resetContent = fse.readFileSync(readMePath, "utf-8");
      assert.equal(resetContent, readmeContent);
    });
  });

  it("can checkout by tree", function() {
    var test = this;

    return test.repository.getTagByName("annotated-tag").then(function(tag) {
      return Checkout.tree(test.repository, tag);
    }).then(function() {
      return test.repository.getHeadCommit();
    }).then(function(commit) {
      assert.equal(commit, "32789a79e71fbc9e04d3eff7425e1771eb595150");
    });
  });

  it("can checkout a branch", function() {
    var test = this;

    return test.repository.checkoutBranch(checkoutBranchName)
    .then(function() {
      var packageContent = fse.readFileSync(packageJsonPath, "utf-8");

      assert.ok(!~packageContent.indexOf("\"ejs\": \"~1.0.0\","));
    })
    .then(function() {
      return test.repository.getStatus();
    })
    .then(function(statuses) {
      assert.equal(statuses.length, 0);
    })
    .then(function() {
      return test.repository.checkoutBranch("master");
    })
    .then(function() {
      var packageContent = fse.readFileSync(packageJsonPath, "utf-8");

      assert.ok(~packageContent.indexOf("\"ejs\": \"~1.0.0\","));
    });
  });

  it("can checkout an index with conflicts", function() {
    const test = this;

    const testBranchName = "test";
    let ourCommit;
    let signature;

    return test.repository.defaultSignature()
    .then((signatureResult) => {
      signature = signatureResult;
      return test.repository.getBranchCommit(checkoutBranchName);
    })
    .then((commit) => {
      ourCommit = commit;

      return test.repository.createBranch(testBranchName, commit.id());
    })
    .then(() => {
      return test.repository.checkoutBranch(testBranchName);
    })
    .then((branch) => {
      fse.writeFileSync(packageJsonPath, "\n");

      return test.repository.refreshIndex()
        .then((index) => {
          return index.addByPath(packageJsonName)
            .then(() => {
              return index.write();
            })
            .then(() => {
              return index.writeTree();
            });
        });
    })
    .then((oid) => {
      assert.equal(oid.toString(),
        "85135ab398976a4d5be6a8704297a45f2b1e7ab2");

      return test.repository.createCommit("refs/heads/" + testBranchName,
        signature, signature, "we made breaking changes", oid, [ourCommit]);
    })
    .then((commit) => {
      return Promise.all([
        test.repository.getBranchCommit(testBranchName),
        test.repository.getBranchCommit("master")
      ]);
    })
    .then((commits) => {
      return NodeGit.Merge.commits(test.repository, commits[0], commits[1],
        null);
    })
    .then((index) => {
      assert.ok(index);
      assert.ok(index.hasConflicts && index.hasConflicts());

      return NodeGit.Checkout.index(test.repository, index);
    })
    .then(() => {
      // Verify that the conflict has been written to disk
      var conflictedContent = fse.readFileSync(packageJsonPath, "utf-8");

      assert.ok(~conflictedContent.indexOf("<<<<<<< ours"));
      assert.ok(~conflictedContent.indexOf("======="));
      assert.ok(~conflictedContent.indexOf(">>>>>>> theirs"));

      // Cleanup
      var opts = {
        checkoutStrategy: Checkout.STRATEGY.FORCE,
        paths: packageJsonName
      };

      return Checkout.head(test.repository, opts);
    })
    .then(() => {
      var finalContent = fse.readFileSync(packageJsonPath, "utf-8");
      assert.equal(finalContent, "\n");
    });
  });
});
