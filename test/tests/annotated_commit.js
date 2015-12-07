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

  it("can free an AnnotatedCommit after creating it", function() {
    var test = this;

    return Branch.lookup(test.repository, branchName, Branch.BRANCH.LOCAL)
      .then(function(ref) {
        return AnnotatedCommit.fromRef(test.repository, ref);
      })
      .then(function(annotatedCommit) {
        // Annotated commit should exist
        assert(annotatedCommit.id());

        // Free the annotated commit
        annotatedCommit.free();

        // Getting the id should now throw because the commit was freed
        assert.throws(annotatedCommit.id);
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
});
