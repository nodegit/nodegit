var assert = require("assert");
var RepoUtils = require("../utils/repository_setup");
var fse = require("fs-extra");
var path = require("path");
var local = path.join.bind(path, __dirname);

var leakTest = require("../utils/leak_test");

describe("Revwalk", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Revwalk = NodeGit.Revwalk;
  var Oid = NodeGit.Oid;

  var reposPath = local("../repos/workdir");

  // Set a reasonable timeout here now that our repository has grown.
  this.timeout(120000);

  beforeEach(function() {
    var test = this;
    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
        return test.repository.getBranchCommit("rev-walk");
      })
      .then(function(commit) {
        test.commit = commit;
      })
      .then(function() {
        test.walker = test.repository.createRevWalk();
        test.walker.sorting(NodeGit.Revwalk.SORT.TIME);
        test.walker.push(test.commit.id());
      });
  });

  it("can create a walker", function() {
    assert.ok(this.walker instanceof Revwalk);
  });

  it("can push an object", function() {
    var sha = this.commit.sha();

    return this.walker.next()
      .then(function(commit) {
        assert.equal(sha, commit);
      });
  });

  it("can hide an object", function() {
    var test = this;

    return next(test.walker, 4)
      .then(function(commit) {
        assert.equal(commit.toString(),
          "b8a94aefb22d0534cc0e5acf533989c13d8725dc");

        test.walker = test.repository.createRevWalk();
        test.walker.push(test.commit.id());
        test.walker.hide(
          Oid.fromString("b8a94aefb22d0534cc0e5acf533989c13d8725dc"));

        return next(test.walker, 3);
      })
      .then(function(commit) {
        assert.equal(commit.toString(),
          "95f695136203a372751c19b6353aeb5ae32ea40e");
          return next(test.walker, 1);
      })
      .then(function(commit) {
        assert.equal(commit, undefined);
      });
  });

  it("can simplify to first parent", function() {
    var test = this;

    test.walker.simplifyFirstParent();
    return next(test.walker, 3)
      .then(function(commit) {
        assert.equal(commit.toString(),
          "b8a94aefb22d0534cc0e5acf533989c13d8725dc");
      });
  });

  it("can get a specified number of commits", function() {
    var test = this;
    var storedCommits;
    return test.walker.getCommits(10)
      .then(function(commits) {
        assert.equal(commits.length, 10);
        storedCommits = commits;
        test.walker = test.repository.createRevWalk();
        test.walker.push(test.commit.id());

        return test.walker.getCommits(8);
      })
      .then(function(commits) {
        assert.equal(commits.length, 8);
        for (var i = 0; i < 8; i++) {
          assert.equal(commits[i].toString(), storedCommits[i].toString());
        }
      });
  });

  it("can get the largest number of commits within a specified range",
    function() {
      var test = this;
      var storedCommits;
      return test.walker.getCommits(991)
        .then(function(commits) {
          assert.equal(commits.length, 990);
          storedCommits = commits;
          test.walker = test.repository.createRevWalk();
          test.walker.push(test.commit.id());
        });
    });

  it("will return all commits from the revwalk if nothing matches", function() {
    var test = this;
    var magicSha = "notintherepoatallwhatsoeverisntthatcool";

    function checkCommit(commit) {
      return commit.toString() != magicSha;
    }

    return test.walker.getCommitsUntil(checkCommit)
      .then(function(commits) {
        assert.equal(commits.length, 990);
      });
  });

  it("can get commits until you tell it not to", function() {
    var test = this;
    var magicSha = "b8a94aefb22d0534cc0e5acf533989c13d8725dc";

    function checkCommit(commit) {
      return commit.toString() != magicSha;
    }

    return test.walker.getCommitsUntil(checkCommit)
      .then(function(commits) {
        assert.equal(commits.length, 4);
        assert.equal(commits[commits.length-1].toString(), magicSha);
      });
  });

  it("can do a fast walk", function() {
    var test = this;
    var magicSha = "b8a94aefb22d0534cc0e5acf533989c13d8725dc";

    return test.walker.fastWalk(10)
      .then(function(commitOids) {
        assert.equal(commitOids.length, 10);
        assert.equal(commitOids[3].toString(), magicSha);
      });
  });

  it("can get the history of a file", function() {
    var test = this;
    var magicShas = [
      "6ed3027eda383d417457b99b38c73f88f601c368",
      "95cefff6aabd3c1f6138ec289f42fec0921ff610",
      "7ad92a7e4d26a1af93f3450aea8b9d9b8069ea8c",
      "96f077977eb1ffcb63f9ce766cdf110e9392fdf5",
      "694adc5369687c47e02642941906cfc5cb21e6c2",
      "eebd0ead15d62eaf0ba276da53af43bbc3ce43ab",
      "1273fff13b3c28cfdb13ba7f575d696d2a8902e1"
    ];

    return test.walker.fileHistoryWalk("include/functions/copy.h", 1000)
      .then(function(results) {
        var shas = results.map(function(result) {
          return result.commit.sha();
        });
        assert.equal(magicShas.length, shas.length);
        magicShas.forEach(function(sha, i) {
          assert.equal(sha, shas[i]);
        });
      });
  });

  it("can get the history of a dir", function() {
    var test = this;
    var magicShas = [
      "6ed3027eda383d417457b99b38c73f88f601c368",
      "95cefff6aabd3c1f6138ec289f42fec0921ff610",
      "7ad92a7e4d26a1af93f3450aea8b9d9b8069ea8c",
      "96f077977eb1ffcb63f9ce766cdf110e9392fdf5",
      "694adc5369687c47e02642941906cfc5cb21e6c2",
      "eebd0ead15d62eaf0ba276da53af43bbc3ce43ab",
      "1273fff13b3c28cfdb13ba7f575d696d2a8902e1",
      "271c65ed16ab147cee715e1076e1d716156cc5a3",
      "94d532004323641fd169f375869c36a82b32fac7",
      "1c71929a905da9faab64472d53815d46ff4391dd",
      "3947245612ae27077517038704b7a679e742658e",
      "a44c81558d0f72ccf6c1facbe2ba0b9b711586a9",
      "01d469416b26340ee4922d5171ef8dbe46c879f4"
    ];

    return test.walker.fileHistoryWalk("include/functions", 1000)
      .then(function(results) {
        var shas = results.map(function(result) {
          return result.commit.sha();
        });
        assert.equal(magicShas.length, shas.length);
        magicShas.forEach(function(sha, i) {
          assert.equal(sha, shas[i]);
        });
      });
  });

  it("can get the history of a file while ignoring parallel branches",
  function() {
    var test = this;
    var magicShas = [
      "f80e085e3118bbd6aad49dad7c53bdc37088bf9b",
      "907b29d8a3b765570435c922a59cd849836a7b51"
    ];
    var shas;
    var walker = test.repository.createRevWalk();
    walker.sorting(NodeGit.Revwalk.SORT.TIME);
    walker.push("115d114e2c4d5028c7a78428f16a4528c51be7dd");

    return walker.fileHistoryWalk("README.md", 15)
      .then(function(results) {
        shas = results.map(function(result) {
          return result.commit.sha();
        });
        assert.equal(magicShas.length, shas.length);
        magicShas.forEach(function(sha, i) {
          assert.equal(sha, shas[i]);
        });

        magicShas = [
          "be6905d459f1b236e44b2445df25aff1783993e9",
          "4a34168b80fe706f52417106821c9cbfec630e47",
          "f80e085e3118bbd6aad49dad7c53bdc37088bf9b",
          "694b2d703a02501f288269bea7d1a5d643a83cc8",
          "907b29d8a3b765570435c922a59cd849836a7b51"
        ];

        walker = test.repository.createRevWalk();
        walker.sorting(NodeGit.Revwalk.SORT.TIME);
        walker.push("d46f7da82969ca6620864d79a55b951be0540bda");

        return walker.fileHistoryWalk("README.md", 50);
      })
      .then(function(results) {
        shas = results.map(function(result) {
          return result.commit.sha();
        });
        assert.equal(magicShas.length, shas.length);
        magicShas.forEach(function(sha, i) {
          assert.equal(sha, shas[i]);
        });
      });
  });

  it("can yield information about renames in a file history walk",
  function() {
    var treeOid;
    var repo;
    var fileNameA = "a.txt";
    var fileNameB = "b.txt";
    var repoPath = local("../repos/renamedFileRepo");
    var signature = NodeGit.Signature.create("Foo bar",
      "foo@bar.com", 123456789, 60);
    var headCommit;

    return RepoUtils.createRepository(repoPath)
      .then(function(r) {
        repo = r;
        return RepoUtils.commitFileToRepo(
          repo,
          fileNameA,
          "line1\nline2\nline3\n"
        );
      })
      .then(function() {
        return fse.move(
          path.join(repoPath, fileNameA),
          path.join(repoPath, fileNameB)
        );
      })
      .then(function() {
        return repo.refreshIndex();
      })
      .then(function(index) {
        return index.addByPath(fileNameB)
          .then(function() {
            return index.removeByPath(fileNameA);
          })
          .then(function() {
            return index.write();
          })
          .then(function() {
            return index.writeTree();
          });
      })
      .then(function(oidResult) {
        treeOid = oidResult;
        return NodeGit.Reference.nameToId(repo, "HEAD");
      })
      .then(function(head) {
        return repo.getCommit(head);
      })
      .then(function(head) {
        return repo.createCommit("HEAD", signature, signature,
          "renamed commit", treeOid, [head]);
      })
      .then(function() {
        return NodeGit.Reference.nameToId(repo, "HEAD");
      })
      .then(function(commitOid) {
        headCommit = commitOid.tostrS();
        var walker = repo.createRevWalk();
        walker.sorting(NodeGit.Revwalk.SORT.TIME);
        walker.push(commitOid.tostrS());
        return walker.fileHistoryWalk(fileNameB, 5);
      })
      .then(function(results) {
        assert.equal(results[0].status, NodeGit.Diff.DELTA.RENAMED);
        assert.equal(results[0].newName, fileNameB);
        assert.equal(results[0].oldName, fileNameA);
      })
      .then(function() {
        var walker = repo.createRevWalk();
        walker.sorting(NodeGit.Revwalk.SORT.TIME);
        walker.push(headCommit);
        return walker.fileHistoryWalk(fileNameA, 5);
      })
      .then(function(results) {
        assert.equal(results[0].status, NodeGit.Diff.DELTA.RENAMED);
        assert.equal(results[0].newName, fileNameB);
        assert.equal(results[0].oldName, fileNameA);
      })
      .then(function() {
        return fse.remove(repoPath);
      });
  });

  it("does not leak", function() {
    var test = this;

    return leakTest(NodeGit.Revwalk, function() {
      const walker = test.repository.createRevWalk();
      walker.push("115d114e2c4d5028c7a78428f16a4528c51be7dd");
      return walker.next();
    });
  });

  // This test requires forcing garbage collection, so mocha needs to be run
  // via node rather than npm, with a la `node --expose-gc [pathtohmoca]
  // [testglob]`
  var testGC = global.gc ? it : it.skip;

  testGC("doesnt segfault when accessing .author() twice", function(done) {
    Repository.open(reposPath).then(function(repository) {
      var walker = repository.createRevWalk();

      repository.getMasterCommit().then(function(firstCommitOnMaster) {
        walker.walk(firstCommitOnMaster.id(), function(err, commit) {
          if (err && err.errno === NodeGit.Error.CODE.ITEROVER) {
            return done();
          }

          for (var i = 0; i < 500; i++) {
            commit.author().name();
            commit.author().email();

            if ( i % 250 === 0) {
              global.gc();
            }
          }
        });
      });
    });
  });

  function next(walker, count) {
    var promise = null;
    for (var i = 0; i < count; i++) {
      if (!promise) {
        promise = walker.next();
      }
      else {
        promise = promise.then(getNext);
      }
    }
    return promise.catch(function(error) {
      if (error && error.errno === NodeGit.Error.CODE.ITEROVER) {
        return Promise.resolve();
      } else {
        throw error;
      }
    });

    function getNext() {
      return walker.next();
    }
  }
});
