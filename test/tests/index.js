var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var fse = require("fs-extra");

describe("Index", function() {
  var IndexUtils = require("../utils/index_setup");
  var RepoUtils = require("../utils/repository_setup");
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var ErrorCodes = NodeGit.Error.CODE;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repo) {
        test.repository = repo;
        return repo.refreshIndex();
      })
      .then(function(index) {
        test.index = index;
      });
  });

  after(function() {
    this.index.clear();
  });

  it("can get the index of a repo and examine entries", function() {
    var entries = this.index.entries();

    assert.equal(entries[0].path, ".gitignore");
  });

  it("can add all entries to the index", function() {
    var repo = this.repository;
    var index = this.index;
    var fileContent = {
      newFile1: "this has some content",
      newFile2: "and this will have more content"
    };
    var fileNames = Object.keys(fileContent);
    var test = this;
    var addCallbacksCount = 0;

    return Promise.all(fileNames.map(function(fileName) {
      return fse.writeFile(
        path.join(repo.workdir(), fileName),
        fileContent[fileName], {});
    }))
    .then(function() {
      return index.addAll(undefined, undefined, function() {
        addCallbacksCount++;
        // ensure that there is no deadlock if we call
        // a sync libgit2 function from the callback
        test.repository.path();

        return 0; // confirm add
      });
    })
    .then(function() {
      assert.equal(addCallbacksCount, 2);

      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path);
      });

      assert.equal(newFiles.length, 2);
    })
    .then(function() {
      return Promise.all(fileNames.map(function(fileName) {
        return fse.remove(path.join(repo.workdir(), fileName));
      }));
    })
    .then(function() {
      return index.clear();
    });
  });

  it("can remove entries from the index", function() {
    var repo = this.repository;
    var index = this.index;
    var fileContent = {
      newFile1: "this has some content",
      newFile2: "and this will have more content",
      differentFileName: "this has a different name and shouldn't be deleted"
    };
    var fileNames = Object.keys(fileContent);
    var removeCallbacksCount = 0;

    return Promise.all(fileNames.map(function(fileName) {
      return fse.writeFile(
        path.join(repo.workdir(), fileName),
        fileContent[fileName], {});
    }))
    .then(function() {
      return index.addAll();
    })
    .then(function() {
      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path);
      });

      assert.equal(newFiles.length, 3);

      return index.removeAll("newFile*", function() {
        removeCallbacksCount++;

        return 0; // confirm remove
      });
    })
    .then(function() {
      assert.equal(removeCallbacksCount, 2);

      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path);
      });

      assert.equal(newFiles.length, 1);
    })
    .then(function() {
      return Promise.all(fileNames.map(function(fileName) {
        return fse.remove(path.join(repo.workdir(), fileName));
      }));
    })
    .then(function() {
      return index.clear();
    });
  });

  it("can update entries in the index", function() {
    var repo = this.repository;
    var index = this.index;
    var fileContent = {
      newFile1: "this has some content",
      newFile2: "and this will have more content"
    };
    var fileNames = Object.keys(fileContent);
    var updateCallbacksCount = 0;

    return Promise.all(fileNames.map(function(fileName) {
      return fse.writeFile(
        path.join(repo.workdir(), fileName),
        fileContent[fileName], {});
    }))
    .then(function() {
      return index.addAll();
    })
    .then(function() {
      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path);
      });

      assert.equal(newFiles.length, 2);

      return fse.remove(path.join(repo.workdir(), fileNames[0]));
    })
    .then(function() {
      return index.updateAll("newFile*", function() {
        updateCallbacksCount++;

        return 0; // confirm update
      });
    })
    .then(function() {
      assert.equal(updateCallbacksCount, 1);

      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path);
      });

      assert.equal(newFiles.length, 1);
      return fse.remove(path.join(repo.workdir(), fileNames[1]));
    });
  });

  it("can get a conflict from the index", function() {
    var fileName = "everyonesFile.txt";
    var rebaseReposPath = local("../repos/rebase");
    var ourBranchName = "ours";
    var theirBranchName = "theirs";

    var baseFileContent = "How do you feel about Toll Roads?\n";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!\n";
    var theirFileContent = "I'm skeptical about Toll Roads\n";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository;
    var ourCommit;
    var ourBranch;
    var theirBranch;

    return Repository.init(rebaseReposPath, 0)
      .then(function(repo) {
        repository = repo;
        return fse.writeFile(path.join(repository.workdir(), fileName),
        baseFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "044704f62399fecbe22da6d7d47b14e52625630e");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "initial commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "80111c46ac73b857a3493b24c81df08639b5de99");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), fileName),
          baseFileContent + theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "b826e989aca7647bea64810f0a2a38acbbdd4c1a");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "b3c355bb606ec7da87174dfa1a0b0c0e3dc97bc0");

        return fse.writeFile(path.join(repository.workdir(), fileName),
          baseFileContent + ourFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "e7fe41bf7c0c28766887a63ffe2f03f624276fbe");

          return repository.createCommit(ourBranch.name(), ourSignature,
            ourSignature, "we made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "28cfeb17f66132edb3c4dacb7ff38e8dd48a1844");

        var opts = {
          checkoutStrategy: NodeGit.Checkout.STRATEGY.FORCE
        };

        return NodeGit.Checkout.head(repository, opts);
      })
      .then(function() {
        return repository.mergeBranches(ourBranchName, theirBranchName);
      })
      .then(function(commit) {
        assert.fail(commit, undefined,
          "The index should have been thrown due to merge conflicts");
      })
      .catch(function(index) {
        assert.ok(index);
        assert.ok(index.hasConflicts());

        return index.conflictGet(fileName);
      })
      .then(function(conflict) {
        var promises = [];

        promises.push(repository.getBlob(conflict.ancestor_out.id)
          .then(function(blob) {
            assert.equal(blob.toString(), baseFileContent);
          }));

        promises.push(repository.getBlob(conflict.our_out.id)
          .then(function(blob) {
            assert.equal(blob.toString(), baseFileContent + ourFileContent);
          }));

        promises.push(repository.getBlob(conflict.their_out.id)
          .then(function(blob) {
            assert.equal(blob.toString(), baseFileContent + theirFileContent);
          }));

        return Promise.all(promises);
      });
  });

  it("can add a conflict to the index", function() {
    var repo;
    var repoPath = local("../repos/index");
    var ourBranchName = "ours";
    var theirBranchName = "theirs";
    var fileName = "testFile.txt";
    var ancestorIndexEntry;
    var ourIndexEntry;
    var theirIndexEntry;

    return RepoUtils.createRepository(repoPath)
      .then(function(_repo) {
        repo = _repo;
        return IndexUtils.createConflict(
          repo,
          ourBranchName,
          theirBranchName,
          fileName
        );
      })
      .then(function(index) {
        assert.ok(index.hasConflicts());
        return index.conflictGet(fileName);
      })
      .then(function(indexEntries) {
        // Store all indexEntries for conflict
        ancestorIndexEntry = indexEntries.ancestor_out;
        ourIndexEntry = indexEntries.our_out;
        theirIndexEntry = indexEntries.their_out;

        // Stage conflicted file
        return RepoUtils.addFileToIndex(repo, fileName);
      })
      .then(function() {
        return repo.index();
      })
      .then(function(index) {
        assert.ok(!index.hasConflicts());
        return index.conflictAdd(
          ancestorIndexEntry,
          ourIndexEntry,
          theirIndexEntry
        );
      })
      .then(function() {
        return repo.index();
      })
      .then(function(index) {
        assert(index.hasConflicts());
      });
  });

  it("can find the specified file in the index", function() {
    var test = this;

    return test.index.find("src/wrapper.cc")
      .then(function(position) {
        assert.notEqual(position, null);
      });
  });

  it("cannot find the specified file in the index", function() {
    var test = this;

    return test.index.find("src/thisisfake.cc")
      .then(function(position) {
        assert.fail("the item should not be found");
      })
      .catch(function(error) {
        assert.strictEqual(error.errno, ErrorCodes.ENOTFOUND);
      });
  });

  it("cannot find the directory in the index", function() {
    var test = this;

    return test.index.find("src")
      .then(function(position) {
        assert.fail("the item should not be found");
      })
      .catch(function(error) {
        assert.strictEqual(error.errno, ErrorCodes.ENOTFOUND);
      });
  });

  it("can find the specified prefix in the index", function() {
    var test = this;

    return test.index.findPrefix("src/")
      .then(function(position) {
        assert.notEqual(position, null);
      });
  });

  it("cannot find the specified prefix in the index", function() {
    var test = this;

    return test.index.find("testing123/")
      .then(function(position) {
        assert.fail("the item should not be found");
      })
      .catch(function(error) {
        assert.strictEqual(error.errno, ErrorCodes.ENOTFOUND);
      });
  });

  it("can find the prefix when a file shares the name", function() {
    var test = this;

    return test.index.find("LICENSE")
      .then(function(position) {
        assert.notEqual(position, null);
      });
  });
});
