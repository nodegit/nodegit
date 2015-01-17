var assert = require("assert");
var path = require("path");
var fs = require("fs");
var exec = require("child_process").exec;
var Diff = require("../../lib/diff");

describe("Diff", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");
  var oid = "fce88902e66c72b5b93e75bdb5ae717038b221f6";

  var Repository = require("../../lib/repository");

  before(function(done) {
    var test = this;
    var newFilePath = path.join(path.resolve("test/repos/workdir"), "wddiff.txt");

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;

      return repository.getBranchCommit("master").then(function(masterCommit) {

        return masterCommit.getTree().then(function(tree) {
          test.masterCommitTree = tree;

          return repository.getCommit(oid).then(function(commit) {
            test.commit = commit;

            return commit.getDiff().then(function(diff) {
              test.diff = diff;
              fs.writeFileSync(newFilePath, "1 line\n2 line\n3 line\n\n4");

              exec("git add wddiff.txt", {cwd: path.resolve("test/repos/workdir")}, function() {
                Diff.treeToWorkdirWithIndex(test.repository, test.masterCommitTree, null)
                .then(function(workdirDiff) {
                    test.workdirDiff = workdirDiff;
                    done();
                });
              });
            });
          });
        });
      });
    });
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

    var oldContent = "\n__Before submitting a pull request, please ensure " +
      "both unit tests and lint checks pass.__\n";
    assert.equal(lines[2].content(), oldContent);
    assert.equal(lines[3].origin(), Diff.LINE.DELETION);
    assert.equal(lines[4].contentLen(), 90);

    var newContent = "__Before submitting a pull request, please ensure " +
      "both that you've added unit tests to cover your shiny new code, " +
      "and that all unit tests and lint checks pass.__\n";
    assert.equal(lines[3].content(), newContent);
    assert.equal(lines[4].origin(), Diff.LINE.ADDITION);
    assert.equal(lines[3].contentLen(), 162);
  });

  it("can diff the workdir with index", function() {
    var patches = this.workdirDiff.patches();
    assert.equal(patches.length, 1);

    var hunks = patches[0].hunks();
    assert.equal(hunks.length, 1);

    var lines = hunks[0].lines();
    assert.equal(lines[0].content(), "1 line\n");
  });
});
