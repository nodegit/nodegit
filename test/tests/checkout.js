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
      return Checkout.tree(test.repository, test.tag);
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
});
