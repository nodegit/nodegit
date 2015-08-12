var assert = require("assert");
var path = require("path");
var promisify = require("promisify-node");
var Promise = require("nodegit-promise");
var fse = promisify(require("fs-extra"));
var local = path.join.bind(path, __dirname);

describe("Diff", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Diff = NodeGit.Diff;

  var reposPath = local("../repos/workdir");
  var oid = "fce88902e66c72b5b93e75bdb5ae717038b221f6";
  var diffFilename = "wddiff.txt";
  var diffFilepath = local("../repos/workdir", diffFilename);

  var moveFromFile = "README.md";
  var moveToFile = "MOVED_README.md";

  var moveFromPath = local("../repos/workdir", moveFromFile);
  var moveToPath = local("../repos/workdir", moveToFile);

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;

      return repository.openIndex();
    })
    .then(function(index) {
      test.index = index;

      return test.repository.getBranchCommit("master");
    })
    .then(function(masterCommit) {
      return masterCommit.getTree();
    })
    .then(function(tree) {
      test.masterCommitTree = tree;

      return test.repository.getCommit(oid);
    })
    .then(function(commit) {
      test.commit = commit;

      return commit.getDiff();
    })
    .then(function(diff) {
      test.diff = diff;

      return fse.writeFile(diffFilepath, "1 line\n2 line\n3 line\n\n4");
    })
    .then(function() {
      return fse.move(moveFromPath, moveToPath);
    })
    .then(function() {
      return Diff.treeToWorkdirWithIndex(
        test.repository,
        test.masterCommitTree,
        { flags: Diff.OPTION.INCLUDE_UNTRACKED }
      );
    })
    .then(function(workdirDiff) {
      test.workdirDiff = workdirDiff;
    })
    .then(function() {
      var opts = {
        flags: Diff.OPTION.INCLUDE_UNTRACKED |
               Diff.OPTION.RECURSE_UNTRACKED_DIRS
      };

      return Diff.indexToWorkdir(test.repository, test.index, opts);
    })
    .then(function(diff) {
      test.indexToWorkdirDiff = diff;
    })
    .then(function() {
      return fse.remove(diffFilepath);
    })
    .then(function() {
      return fse.move(moveToPath, moveFromPath);
    })
    .catch(function(e) {
      return fse.remove(diffFilepath)
        .then(function() {
          return Promise.reject(e);
        });
    });
  });

  it("can walk a DiffList", function() {
    return this.diff[0].patches()
      .then(function(patches) {
        var patch = patches[0];

        assert.equal(patch.oldFile().path(), "README.md");
        assert.equal(patch.newFile().path(), "README.md");
        assert.equal(patch.size(), 1);
        assert.ok(patch.isModified());

        return patch.hunks();
      })
      .then(function(hunks) {
        var hunk = hunks[0];
        assert.equal(hunk.size(), 5);

        return hunk.lines();
      })
      .then(function(lines) {
        assert.equal(lines[0].origin(), Diff.LINE.CONTEXT);
        assert.equal(lines[1].origin(), Diff.LINE.CONTEXT);
        assert.equal(lines[2].origin(), Diff.LINE.CONTEXT);

        var oldContent = "__Before submitting a pull request, please ensure " +
          "both unit tests and lint checks pass.__\n";
        assert.equal(lines[3].rawContent(), oldContent);
        assert.equal(lines[3].origin(), Diff.LINE.DELETION);
        assert.equal(lines[3].contentLen(), 90);

        var newContent = "__Before submitting a pull request, please ensure " +
          "both that you've added unit tests to cover your shiny new code, " +
          "and that all unit tests and lint checks pass.__\n";
        assert.equal(lines[4].rawContent(), newContent);
        assert.equal(lines[4].origin(), Diff.LINE.ADDITION);
        assert.equal(lines[4].contentLen(), 162);
      });
  });

  it("can diff the workdir with index", function() {
    return this.workdirDiff.patches()
      .then(function(patches) {
        assert.equal(patches.length, 3);
        assert(patches[2].isUntracked());

        var oldFile = patches[2].delta.oldFile();
        assert.equal(oldFile.path(), "wddiff.txt");
        assert.equal(oldFile.size(), 0);

        var newFile = patches[2].delta.newFile();
        assert.equal(newFile.path(), "wddiff.txt");
        assert.equal(newFile.size(), 23);
      });
  });

  it("can resolve individual line chages from the patch hunks", function() {
    return this.workdirDiff.patches()
      .then(function(patches) {
        var result = [];
        var hunkPromises = [];

        patches.forEach(function(patch) {
          hunkPromises.push(patch.hunks()
            .then(function(hunks) {
              result.concat(hunks);
            })
          );
        });

        return Promise.all(hunkPromises)
          .then(function() {
            return result;
          });
      })
      .then(function(hunks) {
        var result = [];
        var linePromises = [];

        hunks.forEach(function(hunk) {
          linePromises.push(hunk.lines()
            .then(function(lines) {
              result.concat(lines);
            })
          );
        });

        return Promise.all(linePromises)
          .then(function() {
            return result;
          });
      })
      .then(function(lines) {
        lines.forEach(function(line) {
          assert(!/\n/.exec(line.content()));
          assert(/\n/.exec(line.rawContent()));
        });
      });
  });

  it("can diff the contents of a file to a string", function(done) {
    this.repository.getBranchCommit("master")
      .then(function(commit) {
        return commit.getEntry("LICENSE");
      })
      .then(function(entry) {
        var _entry = entry;
        return _entry.getBlob();
      })
      .then(function(blob) {
        var buffer = "New Text";
        return Diff.blobToBuffer(
          blob,
          null,
          buffer,
          null,
          null,
          null,
          null,
          function(delta, hunk, payload) {
            assert.equal(hunk.oldStart(), 1);
            assert.equal(hunk.oldLines(), 19);
            assert.equal(hunk.newStart(), 1);
            assert.equal(hunk.newLines(), 1);
            assert.equal(
              hunk.header().substring(0, hunk.headerLen() - 1),
              "@@ -1,19 +1 @@"
            );
            done();
          });
      });
  });

  it("can diff with a null tree", function() {
    var repo = this.repository;
    var tree = this.masterCommitTree;
    return Diff.treeToTree(repo, null, tree, null)
      .then(function(diff) {
        return diff.patches();
      })
      .then(function(patches) {
        assert.equal(patches.length, 84);
      });
  });

  it("can diff the initial commit of a repository", function() {
    var repo = this.repository;
    var oid = "99c88fd2ac9c5e385bd1fe119d89c83dce326219"; // First commit
    return repo.getCommit(oid)
      .then(function(commit) {
        return commit.getDiff();
      })
      .then(function(diffs) {
        return diffs[0].patches();
      })
      .then(function(patches) {
        assert.equal(patches.length, 8);
      });
  });

  it("can diff tree to index", function() {
    var repo = this.repository;
    var tree = this.masterCommitTree;
    var index = this.index;
    var opts = { flags: Diff.OPTION.INCLUDE_UNTRACKED };

    return Diff.treeToIndex(repo, tree, index, opts)
      .then(function(diff) {
        return diff.patches();
      })
      .then(function(patches) {
        assert.equal(patches.length, 0);
      });
  });

  it("can diff index to workdir", function() {
    return this.indexToWorkdirDiff.patches()
      .then(function(patches) {
        assert.equal(patches.length, 3);
      });
  });

  // This wasn't working before. It was only passing because the promise chain
  // was broken
  it.skip("can find similar files in a diff", function() {
    var diff = this.indexToWorkdirDiff;
    var opts = {
      flags: Diff.FIND.RENAMES |
             Diff.FIND.RENAMES_FROM_REWRITES |
             Diff.FIND.FOR_UNTRACKED
    };

    return diff.patches()
      .then(function(patches) {
        assert.equal(patches.length, 3);

        return diff.findSimilar(opts);
      })
      .then(function() {
        return diff.patches();
      })
      .then(function(patches) {
        // Renamed file now treated as one diff, so 3 patches -> 2
        assert.equal(patches.length, 2);
      });
  });
});
