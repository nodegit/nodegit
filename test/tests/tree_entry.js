var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("TreeEntry", function() {
  var reposPath = local("../repos/workdir/.git");
  var oid = "5716e9757886eaf38d51c86b192258c960d9cfea";

  var Repository = require(local("../../lib/repository"));
  var Tree = require(local("../../lib/tree"));

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;

      return repository.getCommit(oid).then(function(commit) {
        test.commit = commit;
      });
    });
  });

  it("will fail on a missing file", function() {
    return this.commit.getEntry("test/-entry.js").then(null, function(err) {
      assert.ok(err instanceof Error);
    });
  });

  it("provides the correct sha for a file", function() {
    return this.commit.getEntry("README.md").then(function(entry) {
      var sha = entry.sha();

      assert.equal(sha, "6cb45ba5d32532bf0d1310dc31ca4f20f59964bc");
    });
  });

  it("provides the filename", function() {
    return this.commit.getEntry("test/raw-commit.js").then(function(entry) {
      var name = entry.filename();

      assert.equal(name, "raw-commit.js");
    });
  });

  it("provides the blob representation of the entry", function() {
    return this.commit.getEntry("test/raw-commit.js").then(function(entry) {
      return entry.getBlob().then(function(blob) {
        var size = blob.rawsize();

        assert.equal(size, 2736);
      });
    });
  });

  it("provides the tree the entry is part of", function() {
    return this.commit.getEntry("test").then(function(entry) {
      return entry.getTree().then(function(tree) {
        assert.ok(tree instanceof Tree);
      });
    });
  });

  it("can determine if an entry is a file", function() {
    return this.commit.getEntry("README.md").then(function(entry) {
      var isFile = entry.isFile();

      assert.equal(isFile, true);
    });
  });

  it("can determine if an entry is a directory", function() {
    return this.commit.getEntry("example").then(function(entry) {
      var isFile = entry.isFile();

      assert.equal(isFile, false);
    });
  });
});
