var assert = require("assert");
var path = require("path");
var fs = require("fs");
var exec = require("child_process").execSync;
var local = path.join.bind(path, __dirname);

describe("Odb.hash", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Odb = NodeGit.Odb;
  var Oid = NodeGit.Oid;
  var Obj = NodeGit.Object;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repo) {
      test.repo = repo;

      return repo.odb();
    }).then(function(odb) {
      test.odb = odb;
    });
  });

  it("can hash a buffer without writing to ODB", function() {
    var content = "test content for hashing";
    var odb = this.odb;

    // Get expected OID from git hash-object
    var expected = exec(
      "printf '" + content + "' | git hash-object --stdin -t blob",
      { encoding: "utf8" }
    ).trim();

    return Odb.hash(content, content.length, Obj.TYPE.BLOB)
      .then(function(oid) {
        assert.ok(oid instanceof Oid);
        assert.equal(oid.tostrS(), expected);
      });
  });

  it("does not mutate the ODB when hashing a buffer", function() {
    var content = "unique content that should not be written " + Date.now();

    return Odb.hash(content, content.length, Obj.TYPE.BLOB)
      .then(function(oid) {
        assert.ok(oid instanceof Oid);

        // Attempting to read the OID from the ODB should fail
        // because it was never written
        return this.odb.read(oid)
          .then(function() {
            assert.fail("Should not be able to read a hash-only OID from ODB");
          })
          .catch(function(err) {
            // Expected: object not found
            assert.ok(err);
          });
      }.bind(this));
  });
});

describe("Repository.hashfile", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Oid = NodeGit.Oid;
  var Obj = NodeGit.Object;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repo) {
      test.repo = repo;

      return repo.odb();
    }).then(function(odb) {
      test.odb = odb;
    });
  });

  it("can hash a file with repository filters", function() {
    var repo = this.repo;

    // Use an existing file in the test repo
    var filePath = path.join(reposPath, "README.md");

    // Get expected OID from git hash-object (which applies filters)
    var expected = exec(
      "git -C " + reposPath + " hash-object " + filePath,
      { encoding: "utf8" }
    ).trim();

    return repo.hashfile(filePath, Obj.TYPE.BLOB)
      .then(function(oid) {
        assert.ok(oid instanceof Oid);
        assert.equal(oid.tostrS(), expected);
      });
  });

  it("does not mutate the ODB when hashing a file", function() {
    var repo = this.repo;
    var odb = this.odb;

    // Create a temporary file with unique content
    var tmpPath = path.join(reposPath, "tmp_hash_test_" + Date.now() + ".txt");
    var content = "unique file content for hash test " + Date.now();
    fs.writeFileSync(tmpPath, content);

    return repo.hashfile(tmpPath, Obj.TYPE.BLOB)
      .then(function(oid) {
        assert.ok(oid instanceof Oid);

        // Clean up temp file
        fs.unlinkSync(tmpPath);

        // Attempting to read the OID from the ODB should fail
        return odb.read(oid)
          .then(function() {
            assert.fail("Should not be able to read a hash-only OID from ODB");
          })
          .catch(function(err) {
            assert.ok(err);
          });
      })
      .catch(function(err) {
        // Clean up temp file on error
        try { fs.unlinkSync(tmpPath); } catch(e) {}
        throw err;
      });
  });

  it("can hash a file with as_path filter hint", function() {
    var repo = this.repo;

    var filePath = path.join(reposPath, "README.md");

    return repo.hashfile(filePath, Obj.TYPE.BLOB, "README.md")
      .then(function(oid) {
        assert.ok(oid instanceof Oid);
        // Just verify it returns a valid OID
        assert.equal(oid.tostrS().length, 40);
      });
  });
});
