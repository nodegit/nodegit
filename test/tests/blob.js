var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var fse = require("fs-extra");
var exec = require("../../utils/execPromise");

describe("Blob", function() {
  var NodeGit = require("../../");

  var Oid = NodeGit.Oid;
  var Repository = NodeGit.Repository;
  var Blob = NodeGit.Blob;
  var FileMode = NodeGit.TreeEntry.FILEMODE;

  var reposPath = local("../repos/workdir");
  var oid = "111dd657329797f6165f52f5085f61ac976dcf04";
  var previousCommitOid = "";

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
        commitMessage,
        treeOid,
        [parent]
      );
    });
  }

  before(function() {
    return Repository.open(reposPath)
      .then(function(repository) {
        return repository.getHeadCommit();
      })
      .then(function(commit) {
        previousCommitOid = commit.id();
      });
  });

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;

        return repository.getBlob(oid);
      })
      .then(function(blob) {
        test.blob = blob;
      });
  });

  after(function() {
    return exec("git clean -xdf", {cwd: reposPath})
      .then(function() {
        return exec("git checkout master", {cwd: reposPath});
      })
      .then(function() {
        return exec("git reset --hard " + previousCommitOid, {cwd: reposPath});
      });
  });

  it("can provide content as a buffer", function() {
    var contents = this.blob.content();

    assert.ok(Buffer.isBuffer(contents));
  });

  it("can provide content as a string", function() {
    var contents = this.blob.toString();

    assert.equal(typeof contents, "string");
    assert.equal(contents.slice(0, 7), "@import");
  });

  it("can determine if a blob is not a binary", function() {
    assert.equal(this.blob.filemode(), FileMode.BLOB);
  });

  it("can get a blob with an Oid object", function() {
    var oidObject = Oid.fromString(oid);
    return this.repository.getBlob(oidObject)
      .then(function(blob) {
        assert.equal(blob.id().toString(), oid);
      });
  });

  describe("createFromBuffer", function() {
    it("creates a new blob from the buffer", function() {
      var content = "This is a new buffer";
      var buf = Buffer.from(content, content.length);
      var test = this;

      return Blob.createFromBuffer(test.repository, buf, content.length)
        .then(function(oid) {
          return test.repository.getBlob(oid);
        })
        .then(function(newBlob) {
          assert.equal(newBlob.toString(), content);
        });
    });

    it("creates blob with content equal to length", function() {
      var content = "This is a new buffer";
      var buf = Buffer.from(content, content.length);
      var test = this;

      return Blob.createFromBuffer(test.repository, buf, 2)
        .then(function(oid) {
          return test.repository.getBlob(oid);
        })
        .then(function(newBlob) {
          assert.equal(newBlob.toString(), "Th");
        });
    });

    it("throws an error when repository is null", function() {
      return Blob.createFromBuffer(null, null, 0)
        .catch(function(error) {
          assert.strictEqual(error.message, "Repository repo is required.");
        });
    });

    it("throws an error when buffer is null", function() {
      var test = this;
      return Blob.createFromBuffer(test.repository, null)
        .catch(function(error) {
          assert.strictEqual(error.message, "Buffer buffer is required.");
        });
    });

    it("throws an error when no length is provided", function() {
      var test = this;
      return Blob.createFromBuffer(test.repository, Buffer.from("testing"))
        .catch(function(error) {
          assert.strictEqual(error.message, "Number len is required.");
        });
    });
  });

  describe("createFromDisk", function() {
    var fileName = path.join(reposPath, "testFile.zzz");
    var fileContent = "this is my file content";

    beforeEach(function() {
      return fse.writeFile(fileName, fileContent);
    });

    afterEach(function() {
      return fse.unlink(fileName);
    });

    it("creates a new blob from the file", function() {
      var test = this;

      return Blob.createFromDisk(test.repository, fileName)
        .then(function(oid) {
          return test.repository.getBlob(oid);
        })
        .then(function(newBlob) {
          assert.equal(newBlob.toString(), fileContent);
        });
    });

    it("throws an error when the file cannot be found", function() {
      var test = this;

      return Blob.createFromDisk(test.repository, "aaaaaaaaaa")
        .catch(function(error) {
          assert.equal(error.errno, -3);
        });
    });

    it("throws an error when repository is null", function() {
      return Blob.createFromDisk(null, null, 0)
        .catch(function(error) {
          assert.strictEqual(error.message, "Repository repo is required.");
        });
    });

    it("throws an error when path is null", function() {
      var test = this;
      return Blob.createFromDisk(test.repository, null)
        .catch(function(error) {
          assert.strictEqual(error.message, "String path is required.");
        });
    });
  });

  describe("createFromWorkdir", function() {
    it("creates a blob from the file", function() {
      var fileName = "package.json";
      var filePath = path.join(reposPath, "package.json");
      var test = this;

      return fse.readFile(filePath)
        .then(function(content) {
          test.content = content.toString();
          return Blob.createFromWorkdir(test.repository, fileName);
        })
        .then(function(oid) {
          return test.repository.getBlob(oid);
        })
        .then(function(newBlob) {
          assert.equal(newBlob.toString(), test.content);
        });
    });

    it("throws an error when the file cannot be found", function() {
      var test = this;

      return Blob.createFromWorkdir(test.repository, "thisisabadfile.jpg")
        .catch(function(error) {
          assert.equal(error.errno, -3);
        });
    });

    it("throws an error when repository is null", function() {
      return Blob.createFromWorkdir(null, null, 0)
        .catch(function(error) {
          assert.strictEqual(error.message, "Repository repo is required.");
        });
    });

    it("throws an error when path is null", function() {
      var test = this;
      return Blob.createFromWorkdir(test.repository, null)
        .catch(function(error) {
          assert
            .strictEqual(error.message, "String relative_path is required.");
        });
    });
  });

  describe("filteredContent (DEPRECATED)", function() {
    var attrFileName = ".gitattributes";
    var filter = "*    text eol=crlf";
    var lineEndingRegex = /\r\n|\r|\n/;
    var newFileName = "testfile.test";

    it("retrieves the filtered content", function() {
      var test = this;

      return commitFile(
        test.repository,
        attrFileName,
        filter,
        "added gitattributes")
        .then(function() {
          return commitFile(
            test.repository,
            newFileName,
            "this\nis\nfun\nguys",
            "added LF ending file"
          );
        })
        .then(function(oid) {
          return test.repository.getCommit(oid);
        })
        .then(function(commit) {
          test.filteredCommit = commit;
          return commit.getEntry(newFileName);
        })
        .then(function(entry) {
          return entry.getBlob();
        })
        .then(function(lfBlob) {
          test.lfBlob = lfBlob;
          var ending = test.lfBlob.toString().match(lineEndingRegex);
          assert.strictEqual(ending[0], "\n");

          return Blob.filteredContent(
            test.lfBlob,
            newFileName,
            0
          );
        })
        .then(function(content) {
          var ending = content.match(lineEndingRegex);
          assert.strictEqual(ending[0], "\r\n");
          assert.notStrictEqual(content, test.blob.toString());
        });
    });

    it("returns non-binary filtered content when checking binary", function() {
      var test = this;

      return commitFile(
        test.repository,
        attrFileName,
        filter,
        "added gitattributes")
        .then(function() {
          return commitFile(
            test.repository,
            newFileName,
            "this\nis\nfun\nguys",
            "added LF ending file"
          );
        })
        .then(function(oid) {
          return test.repository.getCommit(oid);
        })
        .then(function(commit) {
          test.filteredCommit = commit;
          return commit.getEntry(newFileName);
        })
        .then(function(entry) {
          return entry.getBlob();
        })
        .then(function(lfBlob) {
          test.lfBlob = lfBlob;
          var ending = test.lfBlob.toString().match(lineEndingRegex);
          assert.strictEqual(ending[0], "\n");

          return Blob.filteredContent(
            test.lfBlob,
            newFileName,
            1
          );
        })
        .then(function(content) {
          var ending = content.match(lineEndingRegex);
          assert.strictEqual(ending[0], "\r\n");
          assert.notStrictEqual(content, test.blob.toString());
        });
    });

    it("returns nothing when checking binary blob", function() {
      var test = this;
      var binary = Buffer.from(new Uint8Array([1,2,3,4,5,6]));

      return commitFile(
        test.repository,
        attrFileName,
        filter,
        "added gitattributes")
        .then(function() {
          return commitFile(
            test.repository,
            newFileName,
            binary,
            "binary content"
          );
        })
        .then(function(oid) {
          return test.repository.getCommit(oid);
        })
        .then(function(commit) {
          test.filteredCommit = commit;
          return commit.getEntry(newFileName);
        })
        .then(function(entry) {
          return entry.getBlob();
        })
        .then(function(binaryBlob) {
          test.binaryBlob = binaryBlob;
          assert.equal(true, binaryBlob.isBinary());

          return Blob.filteredContent(
            test.binaryBlob,
            newFileName,
            1
          );
        })
        .then(function(content) {
          assert.strictEqual(content, "");
        });
    });

    it("returns blob when not checking binary on binary blob", function() {
      var test = this;
      var binary = Buffer.from(new Uint8Array([1,2,3,4,5,6]));

      return commitFile(
        test.repository,
        attrFileName,
        filter,
        "added gitattributes")
        .then(function() {
          return commitFile(
            test.repository,
            newFileName,
            binary,
            "binary content"
          );
        })
        .then(function(oid) {
          return test.repository.getCommit(oid);
        })
        .then(function(commit) {
          test.filteredCommit = commit;
          return commit.getEntry(newFileName);
        })
        .then(function(entry) {
          return entry.getBlob();
        })
        .then(function(binaryBlob) {
          test.binaryBlob = binaryBlob;
          assert.equal(true, binaryBlob.isBinary());

          return Blob.filteredContent(
            test.binaryBlob,
            newFileName,
            0
          );
        })
        .then(function(content) {
          assert.strictEqual(content, binary.toString());
        });
    });

    it("throws an error when the blob is null", function() {
      return Blob.filteredContent(null, "", 0)
        .catch(function(err) {
          assert.strictEqual(
            err.message,
            "Blob blob is required."
          );
        });
    });

    it("throws an error when the path is null", function() {
      var test = this;
      return Blob.filteredContent(test.blob, null, 0)
        .catch(function(err) {
          assert.strictEqual(err.message, "String as_path is required.");
        });
    });

    it("throws an error when the flag is undefined", function() {
      var test = this;
      return Blob.filteredContent(test.blob, "")
        .catch(function(err) {
          assert.strictEqual(
            err.message,
            "Number check_for_binary_data is required."
          );
        });
    });
  });

  describe("filter", function() {
    var attrFileName = ".gitattributes";
    var filter = "*    text eol=crlf";
    var lineEndingRegex = /\r\n|\r|\n/;
    var newFileName = "testfile.test";

    it("retrieves the filtered content", function() {
      var test = this;

      return commitFile(
        test.repository,
        attrFileName,
        filter,
        "added gitattributes")
        .then(function() {
          return commitFile(
            test.repository,
            newFileName,
            "this\nis\nfun\nguys",
            "added LF ending file"
          );
        })
        .then(function(oid) {
          return test.repository.getCommit(oid);
        })
        .then(function(commit) {
          test.filteredCommit = commit;
          return commit.getEntry(newFileName);
        })
        .then(function(entry) {
          return entry.getBlob();
        })
        .then(function(lfBlob) {
          test.lfBlob = lfBlob;
          var ending = test.lfBlob.toString().match(lineEndingRegex);
          assert.strictEqual(ending[0], "\n");

          return test.lfBlob.filter(newFileName, { flags: 0 });
        })
        .then(function(content) {
          var ending = content.match(lineEndingRegex);
          assert.strictEqual(ending[0], "\r\n");
          assert.notStrictEqual(content, test.blob.toString());
        });
    });

    it("returns non-binary filtered content when checking binary", function() {
      var test = this;

      return commitFile(
        test.repository,
        attrFileName,
        filter,
        "added gitattributes")
        .then(function() {
          return commitFile(
            test.repository,
            newFileName,
            "this\nis\nfun\nguys",
            "added LF ending file"
          );
        })
        .then(function(oid) {
          return test.repository.getCommit(oid);
        })
        .then(function(commit) {
          test.filteredCommit = commit;
          return commit.getEntry(newFileName);
        })
        .then(function(entry) {
          return entry.getBlob();
        })
        .then(function(lfBlob) {
          test.lfBlob = lfBlob;
          var ending = test.lfBlob.toString().match(lineEndingRegex);
          assert.strictEqual(ending[0], "\n");

          return test.lfBlob.filter(
            newFileName,
            { flags: NodeGit.Blob.FILTER_FLAG.CHECK_FOR_BINARY }
          );
        })
        .then(function(content) {
          var ending = content.match(lineEndingRegex);
          assert.strictEqual(ending[0], "\r\n");
          assert.notStrictEqual(content, test.blob.toString());
        });
    });

    it("returns nothing when checking binary blob", function() {
      var test = this;
      var binary = Buffer.from(new Uint8Array([1,2,3,4,5,6]));

      return commitFile(
        test.repository,
        attrFileName,
        filter,
        "added gitattributes")
        .then(function() {
          return commitFile(
            test.repository,
            newFileName,
            binary,
            "binary content"
          );
        })
        .then(function(oid) {
          return test.repository.getCommit(oid);
        })
        .then(function(commit) {
          test.filteredCommit = commit;
          return commit.getEntry(newFileName);
        })
        .then(function(entry) {
          return entry.getBlob();
        })
        .then(function(binaryBlob) {
          test.binaryBlob = binaryBlob;
          assert.equal(true, binaryBlob.isBinary());

          return test.binaryBlob.filter(
            newFileName,
            { flags: NodeGit.Blob.FILTER_FLAG.CHECK_FOR_BINARY }
          );
        })
        .then(function(content) {
          assert.strictEqual(content, "");
        });
    });

    it("returns blob when not checking binary on binary blob", function() {
      var test = this;
      var binary = Buffer.from(new Uint8Array([1,2,3,4,5,6]));

      return commitFile(
        test.repository,
        attrFileName,
        filter,
        "added gitattributes")
        .then(function() {
          return commitFile(
            test.repository,
            newFileName,
            binary,
            "binary content"
          );
        })
        .then(function(oid) {
          return test.repository.getCommit(oid);
        })
        .then(function(commit) {
          test.filteredCommit = commit;
          return commit.getEntry(newFileName);
        })
        .then(function(entry) {
          return entry.getBlob();
        })
        .then(function(binaryBlob) {
          test.binaryBlob = binaryBlob;
          assert.equal(true, binaryBlob.isBinary());

          return test.binaryBlob.filter(
            newFileName,
            { flags: 0 }
          );
        })
        .then(function(content) {
          assert.strictEqual(content, binary.toString());
        });
    });

    it("throws an error when the path is null", function() {
      var test = this;
      return test.blob.filter(test.blob, null, { flags: 0 })
        .catch(function(err) {
          assert.strictEqual(err.message, "String as_path is required.");
        });
    });
  });
});
