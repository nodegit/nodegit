var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

var leakTest = require("../utils/leak_test");

describe("TreeEntry", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Tree = NodeGit.Tree;

  var reposPath = local("../repos/workdir");
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

  it("provides the correct length for a file", function() {
    return this.commit.getEntry("README.md")
	  .then(function(entry) {
              assert.equal(entry.name().length, 9);
	  });
  });

  it("provides the filename", function() {
    return this.commit.getEntry("test/raw-commit.js")
      .then(function(entry) {
        assert.equal(entry.name(), "raw-commit.js");
    });
  });

  it("provides the full path", function() {
    return this.commit.getEntry("test/raw-commit.js")
      .then(function(entry) {
        assert.equal(entry.path(), "test/raw-commit.js");
      });
  });

  it("provides the full path when the entry came from a tree", function() {
    var testTree = function(tree, _dir) {
      var dir = _dir || "",
        testPromises = [];
      tree.entries().forEach(function(entry) {
        var currentPath = path.posix.join(dir, entry.name());
        if (entry.isTree()) {
          testPromises.push(
            entry.getTree().then(function (subtree) {
              return testTree(subtree, currentPath);
            })
          );
        } else {
          assert.equal(entry.path(), currentPath);
        }
      });

      return Promise.all(testPromises);
    };

    return this.commit.getTree()
      .then(testTree);
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

  it("provides the blob representation via callback", function() {
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

  it("can determine if an entry is not a file", function() {
    return this.commit.getEntry("example")
      .then(function(entry) {
        assert.equal(entry.isFile(), false);
      });
  });

  it("can determine if an entry is a directory", function() {
    return this.commit.getEntry("example")
      .then(function(entry) {
        assert.equal(entry.isDirectory(), true);
      });
  });

  it("can determine if an entry is a submodule", function() {
    var repo = this.repository;
    return repo.getCommit("878ef6efbc5f85c4f63aeedf41addc262a621308")
      .then(function(commit) {
        return commit.getEntry("vendor/libgit2")
        .then(function(entry) {
          assert.equal(entry.isSubmodule(), true);
      });
    });
  });

  it("can determine if an entry is not a submodule", function() {
    return this.commit.getEntry("example")
      .then(function(entry) {
        assert.equal(entry.isSubmodule(), false);
      });
  });

  it("can convert entry into a blob", function() {
    var repo = this.repository;
    return this.commit.getEntry("README.md")
      .then(function(entry) {
        return entry.toObject(repo);
      })
      .then(function(object) {
        assert.equal(object.isBlob(), true);
      });
  });

  it("can convert entry into a tree", function() {
    var repo = this.repository;
    return this.commit.getEntry("example")
      .then(function(entry) {
        return entry.toObject(repo);
      })
      .then(function(object) {
        assert.equal(object.isTree(), true);
      });
  });

  it("does not leak", function() {
    var test = this;

    return leakTest(NodeGit.TreeEntry, function() {
      return test.commit.getTree()
        .then(function(tree) {
          return tree.entryByPath("example");
        });
    });
  });
});
