var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("AnnotatedCommit", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var AnnotatedCommit = NodeGit.AnnotatedCommit;
  var Branch = NodeGit.Branch;

  var reposPath = local("../repos/workdir");
  var branchName = "master";

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      });
  });

  it("can create an AnnotatedCommit from a ref", function() {
    var test = this;

    return Branch.lookup(test.repository, branchName, Branch.BRANCH.LOCAL)
      .then(function(ref) {
        return AnnotatedCommit.fromRef(test.repository, ref);
      })
      .then(function(annotatedCommit) {
        assert(annotatedCommit.id());
      });
  });

  it("can lookup an AnnotatedCommit after creating it", function() {
    var test = this;
    var id;

    return Branch.lookup(test.repository, branchName, Branch.BRANCH.LOCAL)
      .then(function(ref) {
        return AnnotatedCommit.fromRef(test.repository, ref);
      })
      .then(function(annotatedCommit) {
        id = annotatedCommit.id();
        return AnnotatedCommit.lookup(test.repository, id);
      })
      .then(function(annotatedCommit) {
        assert(id, annotatedCommit.id());
      });
  });

  it("can lookup an AnnotatedCommit from a revspec", function() {
    var test = this;

    return AnnotatedCommit.fromRevspec(test.repository, "checkout-test")
      .then(function(annotatedCommit) {
        assert.equal(annotatedCommit.id().toString(),
          "1729c73906bb8467f4095c2f4044083016b4dfde");
      });
  });

  it("can lookup an AnnotatedCommit from a fetchhead", function() {
    var test = this;

    return AnnotatedCommit.fromFetchhead(test.repository,
      "rev-walk",
      "https://github.com/nodegit/test",
      "32789a79e71fbc9e04d3eff7425e1771eb595150")
      .then(function(annotatedCommit) {
        assert.equal(annotatedCommit.id().toString(),
          "32789a79e71fbc9e04d3eff7425e1771eb595150");
      });
  });
});
