var assert = require("assert");
var RepoUtils = require("../utils/repository_setup");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Mempack", function() {
  var NodeGit = require("../../");

  var repoPath = local("../repos/mempackRepo");

  beforeEach(function() {
    var test = this;

    return RepoUtils.createRepository(repoPath)
      .then(function(repository) {
        test.repository = repository;
      });
  });

  it("can create a mempack backend", function() {
    return NodeGit.Mempack.create()
      .then(function(backend) {
        assert.ok(backend, "Mempack backend should be created");
      });
  });

  it("can add mempack backend to ODB and write objects in memory", function() {
    var test = this;
    var mempackBackend;

    return NodeGit.Mempack.create()
      .then(function(backend) {
        mempackBackend = backend;
        return test.repository.odb();
      })
      .then(function(odb) {
        test.odb = odb;
        return odb.addMempackBackend(mempackBackend, 999);
      })
      .then(function() {
        var content = "Hello from mempack test!\n";
        return test.odb.write(content, content.length, NodeGit.Object.TYPE.BLOB);
      })
      .then(function(blobOid) {
        assert.ok(blobOid, "Blob OID should be returned");
        var oidStr = blobOid.tostrS();
        assert.ok(
          /^[0-9a-f]{40}$/.test(oidStr),
          "Blob OID should be a valid SHA-1 hash, got: " + oidStr
        );
      });
  });

  it("can write a tree via in-memory index without disk writes", function() {
    var test = this;
    var mempackBackend;
    var odb;

    return NodeGit.Mempack.create()
      .then(function(backend) {
        mempackBackend = backend;
        return test.repository.odb();
      })
      .then(function(_odb) {
        odb = _odb;
        return odb.addMempackBackend(mempackBackend, 999);
      })
      .then(function() {
        return NodeGit.Index.create();
      })
      .then(function(index) {
        test.index = index;
        var content = "Hello from mempack test!\n";
        return odb.write(content, content.length, NodeGit.Object.TYPE.BLOB);
      })
      .then(function(blobOid) {
        var entry = new NodeGit.IndexEntry();
        entry.path = "test-file.txt";
        entry.id = blobOid;
        entry.mode = 0o100644;
        entry.fileSize = 24;
        entry.flags = 0;
        entry.flagsExtended = 0;

        return test.index.add(entry);
      })
      .then(function() {
        return test.index.writeTreeTo(test.repository);
      })
      .then(function(treeOid) {
        var oidStr = treeOid.tostrS();
        assert.ok(
          /^[0-9a-f]{40}$/.test(oidStr),
          "Tree OID should be a valid SHA-1 hash, got: " + oidStr
        );
      });
  });

  it("can reset the mempack backend", function() {
    var test = this;
    var mempackBackend;

    return NodeGit.Mempack.create()
      .then(function(backend) {
        mempackBackend = backend;
        return test.repository.odb();
      })
      .then(function(odb) {
        return odb.addMempackBackend(mempackBackend, 999);
      })
      .then(function() {
        return mempackBackend.reset();
      })
      .then(function() {
        assert.ok(true, "Mempack reset completed successfully");
      });
  });
});
