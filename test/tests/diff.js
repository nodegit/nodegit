var assert = require("assert");
var path = require("path");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));
var Diff = require("../../lib/diff");
var normalizeOptions = require("../../lib/util/normalize_options");
var NodeGit = require("../../");

describe("Diff", function() {
  var Repository = require("../../lib/repository");
  var reposPath = path.resolve("test/repos/workdir/.git");
  var oid = "fce88902e66c72b5b93e75bdb5ae717038b221f6";
  var diffFilename = "wddiff.txt";
  var diffFilepath = path.join(
    path.resolve("test/repos/workdir"),
    diffFilename
  );

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;

      return repository.getBranchCommit("master");
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
    }).then(function(diff) {
      test.diff = diff;

      return fse.writeFile(diffFilepath, "1 line\n2 line\n3 line\n\n4");
    })
    .then(function() {
      return Diff.treeToWorkdirWithIndex(
        test.repository,
        test.masterCommitTree,
        normalizeOptions({
          flags: Diff.OPTION.INCLUDE_UNTRACKED
        }, NodeGit.DiffOptions)
      );
    })
    .then(function(workdirDiff) {
      test.workdirDiff = workdirDiff;
    });
  });

  after(function() {
    return fse.unlink(diffFilepath);
  });

  it("can walk a DiffList", function() {
    var patch = this.diff[0].patches()[0];

    assert.equal(patch.oldFile().path(), "README.md");
    assert.equal(patch.newFile().path(), "README.md");
    assert.equal(patch.size(), 1);
    assert.ok(patch.isModified());

    var hunk = patch.hunks()[0];
    assert.equal(hunk.size(), 5);

    var lines = hunk.lines();
    assert.equal(lines[0].origin(), Diff.LINE.CONTEXT);
    assert.equal(lines[1].origin(), Diff.LINE.CONTEXT);
    assert.equal(lines[2].origin(), Diff.LINE.CONTEXT);

    var oldContent = "__Before submitting a pull request, please ensure " +
      "both unit tests and lint checks pass.__\n";
    assert.equal(lines[3].content(), oldContent);
    assert.equal(lines[3].origin(), Diff.LINE.DELETION);
    assert.equal(lines[3].contentLen(), 90);

    var newContent = "__Before submitting a pull request, please ensure " +
      "both that you've added unit tests to cover your shiny new code, " +
      "and that all unit tests and lint checks pass.__\n";
    assert.equal(lines[4].content(), newContent);
    assert.equal(lines[4].origin(), Diff.LINE.ADDITION);
    assert.equal(lines[4].contentLen(), 162);
  });

  it("can diff the workdir with index", function() {
    var patches = this.workdirDiff.patches();
    assert.equal(patches.length, 1);
    assert(patches[0].isUntracked());

    var oldFile = patches[0].delta.oldFile();
    assert.equal(oldFile.path(), "wddiff.txt");
    assert.equal(oldFile.size(), 0);

    var newFile = patches[0].delta.newFile();
    assert.equal(newFile.path(), "wddiff.txt");
    assert.equal(newFile.size(), 23);
  });

  it("can diff with a null tree", function() {
    var repo = this.repository;
    var tree = this.masterCommitTree;
    return Diff.treeToTree(repo, null, tree, null).then(function(diff) {
      assert.equal(diff.patches().length, 85);
    });
  });

  it("can diff the initial commit of a repository", function() {
    var repo = this.repository;
    var oid = "99c88fd2ac9c5e385bd1fe119d89c83dce326219"; // First commit
    return repo.getCommit(oid).then(function(commit) {
      return commit.getDiff().then(function(diffs) {
        assert.equal(diffs[0].patches().length, 8);
      });
    });
  });
});
