var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("TreeEntry", function() {
  var Repository = require(local("../../lib/repository"));
  var Tree = require(local("../../lib/tree"));

  var reposPath = local("../repos/workdir/.git");
  var oid = "5716e9757886eaf38d51c86b192258c960d9cfea";

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;

        return repository.getCommit(oid);
      })
      .then(function(commit) {
        test.commit = commit;
      });
  });

  it("will fail on a missing file", function() {
    return this.commit.getEntry("test/-entry.js")
      .then(null, function(err) {
        assert.ok(err instanceof Error);
      });
  });

  it("provides the correct sha for a file", function() {
    return this.commit.getEntry("README.md")
      .then(function(entry) {
        assert.equal(entry.sha(), "6cb45ba5d32532bf0d1310dc31ca4f20f59964bc");
      });
  });

  it("provides the filename", function() {
    return this.commit.getEntry("test/raw-commit.js")
      .then(function(entry) {
        assert.equal(entry.filename(), "raw-commit.js");
    });
  });

  it("provides the blob representation of the entry", function() {
    return this.commit.getEntry("test/raw-commit.js")
      .then(function(entry) {
        return entry.getBlob();
      })
      .then(function(blob) {
        assert.equal(blob.rawsize(), 2736);
      });
  });

  it("provides the blob representation viacallback", function() {
    return this.commit.getEntry("test/raw-commit.js")
      .then(function(entry) {
        entry.getBlob(function (error, blob) {
          assert.equal(blob.rawsize(), 2736);
        });
      });
  });

  it("provides the tree the entry is part of", function() {
    return this.commit.getEntry("test")
      .then(function(entry) {
        return entry.getTree();
      })
      .then(function(tree) {
        assert.ok(tree instanceof Tree);
      });
  });

  it("can determine if an entry is a file", function() {
    return this.commit.getEntry("README.md")
      .then(function(entry) {
        assert.ok(entry.isFile());
      });
  });

  it("can determine if an entry is a directory", function() {
    return this.commit.getEntry("example")
      .then(function(entry) {
        assert.equal(entry.isFile(), false);
      });
  });
});
