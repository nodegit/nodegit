var assert = require("assert");
var path = require("path");

describe("Diff", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");
  var oid = "fce88902e66c72b5b93e75bdb5ae717038b221f6";

  var Repository = require("../../lib/repository");
  var Diff = require("../../lib/diff");

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;

      return repository.getCommit(oid).then(function(commit) {
        test.commit = commit;

        return commit.getDiff().then(function(diff) {
          test.diff = diff;
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
    assert.equal(lines[0].origin(), Diff.LineOrigin.Context);
    assert.equal(lines[1].origin(), Diff.LineOrigin.Context);
    assert.equal(lines[2].origin(), Diff.LineOrigin.Context);

    var oldContent = "\n__Before submitting a pull request, please ensure " +
      "both unit tests and lint checks pass.__\n";
    assert.equal(lines[2].content(), oldContent);
    assert.equal(lines[3].origin(), Diff.LineOrigin.Deletion);
    assert.equal(lines[4].contentLen(), 90);

    var newContent = "__Before submitting a pull request, please ensure " +
      "both that you've added unit tests to cover your shiny new code, " +
      "and that all unit tests and lint checks pass.__\n";
    assert.equal(lines[3].content(), newContent);
    assert.equal(lines[4].origin(), Diff.LineOrigin.Addition);
    assert.equal(lines[3].contentLen(), 162);
  });
});
