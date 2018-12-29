var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var fse = require("fs-extra");

describe("Reset", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Reset = NodeGit.Reset;
  var AnnotatedCommit = NodeGit.AnnotatedCommit;

  var reposPath = local("../repos/workdir");
  var currentCommitOid = "32789a79e71fbc9e04d3eff7425e1771eb595150";
  var previousCommitOid = "c82fb078a192ea221c9f1093c64321c60d64aa0d";
  var filePath = "package.json";

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repo = repository;

        return test.repo.getCommit(currentCommitOid);
      })
      .then(function(commit) {
        test.currentCommit = commit;

        return commit.getEntry(filePath);
      })
      .then(function(entry) {
        return entry.getBlob();
      })
      .then(function(blob) {
        test.currentCommitBlob = blob;

        return test.repo.getCommit(previousCommitOid);
      })
      .then(function(commit) {
        test.previousCommit = commit;

        return commit.getEntry(filePath);
      })
      .then(function(entry) {
        return entry.getBlob();
      })
      .then(function(blob) {
        test.previousCommitBlob = blob;
      });
  });

  it("can reset a file to a previous commit", function() {
    var test = this;

    return Reset.default(test.repo, test.previousCommit, filePath)
    .then(function() {
      return test.repo.refreshIndex();
    })
    .then(function(index) {
      return index.writeTree();
    })
    .then(function(oid) {
      return test.repo.getTree(oid);
    })
    .then(function(tree) {
      return tree.getEntry(filePath);
    })
    .then(function(entry) {
      return entry.getBlob();
    })
    .then(function(blob) {
      var currentCommitContents = test.currentCommitBlob.toString();
      var previousCommitContents = test.previousCommitBlob.toString();
      var resetContents = blob.toString();

      assert(resetContents != currentCommitContents);
      assert(resetContents == previousCommitContents);
    })
    .then(function() {
      return Reset.default(test.repo, test.currentCommit, filePath);
    })
    .then(function() {
      return test.repo.refreshIndex();
    })
    .then(function(index) {
      return index.writeTree();
    })
    .then(function(oid) {
      return test.repo.getTree(oid);
    })
    .then(function(tree) {
      return tree.getEntry(filePath);
    })
    .then(function(entry) {
      return entry.getBlob();
    })
    .then(function(blob) {
      var currentCommitContents = test.currentCommitBlob.toString();
      var previousCommitContents = test.previousCommitBlob.toString();
      var resetContents = blob.toString();

      assert(resetContents == currentCommitContents);
      assert(resetContents != previousCommitContents);
    });
  });

  function resetFrom(repo, commit, resetType, annotated) {
    var promise = null;
    if (annotated) {
      promise = AnnotatedCommit.lookup(repo, commit.id())
      .then(function(annotatedCommit) {
        return Reset.fromAnnotated(repo, annotatedCommit, resetType);
      });
    } else {
      promise = Reset.reset(repo, commit, resetType);
    }
    return promise
    .then(function() {
      return repo.refreshIndex();
    })
    .then(function(index) {
      return index.writeTree();
    })
    .then(function(oid) {
      return repo.getTree(oid);
    })
    .then(function(tree) {
      return tree.getEntry(filePath);
    })
    .then(function(entry) {
      return entry.getBlob();
    });
  }

  it("can perform a soft reset", function() {
    var test = this;

    return resetFrom(test.repo, test.previousCommit, Reset.TYPE.SOFT, false)
    .then(function(blob) {
      var currentCommitContents = test.currentCommitBlob.toString();
      var previousCommitContents = test.previousCommitBlob.toString();
      var resetContents = blob.toString();

      // With a soft reset all of the changes should be in the index
      // still so the index should still == what we had at the current
      // commit and not the one nwe reset to
      assert(resetContents == currentCommitContents);
      assert(resetContents != previousCommitContents);

      return Reset(test.repo, test.currentCommit, Reset.TYPE.HARD);
    });
  });

  it("can perform an annotated soft reset", function() {
    var test = this;

    return resetFrom(test.repo, test.previousCommit, Reset.TYPE.SOFT, true)
    .then(function(blob) {
      var currentCommitContents = test.currentCommitBlob.toString();
      var previousCommitContents = test.previousCommitBlob.toString();
      var resetContents = blob.toString();

      // With a soft reset all of the changes should be in the index
      // still so the index should still == what we had at the current
      // commit and not the one nwe reset to
      assert(resetContents == currentCommitContents);
      assert(resetContents != previousCommitContents);

      return Reset(test.repo, test.currentCommit, Reset.TYPE.HARD);
    });
  });

  it("can perform a mixed reset", function() {
    var test = this;

    return resetFrom(test.repo, test.previousCommit, Reset.TYPE.MIXED, false)
    .then(function(blob) {
      var currentCommitContents = test.currentCommitBlob.toString();
      var previousCommitContents = test.previousCommitBlob.toString();
      var resetContents = blob.toString();

      // With a mixed reset all of the changes should removed from the index
      // but still in the working directory. (i.e. unstaged)
      assert(resetContents != currentCommitContents);
      assert(resetContents == previousCommitContents);

      return fse.readFile(path.join(test.repo.workdir(), filePath));
    })
    .then(function(fileContents) {
      var currentCommitContents = test.currentCommitBlob.toString();

      assert(fileContents == currentCommitContents);

      return Reset.reset(test.repo, test.currentCommit, Reset.TYPE.HARD);
    });
  });

  it("can perform an annotated mixed reset", function() {
    var test = this;

    return resetFrom(test.repo, test.previousCommit, Reset.TYPE.MIXED, true)
    .then(function(blob) {
      var currentCommitContents = test.currentCommitBlob.toString();
      var previousCommitContents = test.previousCommitBlob.toString();
      var resetContents = blob.toString();

      // With a mixed reset all of the changes should removed from the index
      // but still in the working directory. (i.e. unstaged)
      assert(resetContents != currentCommitContents);
      assert(resetContents == previousCommitContents);

      return fse.readFile(path.join(test.repo.workdir(), filePath));
    })
    .then(function(fileContents) {
      var currentCommitContents = test.currentCommitBlob.toString();

      assert(fileContents == currentCommitContents);

      return Reset.reset(test.repo, test.currentCommit, Reset.TYPE.HARD);
    });
  });

  it("can perform a hard reset", function() {
    var test = this;

    return resetFrom(test.repo, test.previousCommit, Reset.TYPE.HARD, false)
    .then(function(blob) {
      var currentCommitContents = test.currentCommitBlob.toString();
      var previousCommitContents = test.previousCommitBlob.toString();
      var resetContents = blob.toString();

      // With a hard reset all of the changes should removed from the index
      // and also removed from the working directory
      assert(resetContents != currentCommitContents);
      assert(resetContents == previousCommitContents);

      return fse.readFile(path.join(test.repo.workdir(), filePath));
    })
    .then(function(fileContents) {
      var previousCommitContents = test.previousCommitBlob.toString();

      assert(fileContents == previousCommitContents);

      return Reset.reset(test.repo, test.currentCommit, Reset.TYPE.HARD);
    });
  });

  it("can perform an annotated hard reset", function() {
    var test = this;

    return resetFrom(test.repo, test.previousCommit, Reset.TYPE.HARD, true)
    .then(function(blob) {
      var currentCommitContents = test.currentCommitBlob.toString();
      var previousCommitContents = test.previousCommitBlob.toString();
      var resetContents = blob.toString();

      // With a hard reset all of the changes should removed from the index
      // and also removed from the working directory
      assert(resetContents != currentCommitContents);
      assert(resetContents == previousCommitContents);

      return fse.readFile(path.join(test.repo.workdir(), filePath));
    })
    .then(function(fileContents) {
      var previousCommitContents = test.previousCommitBlob.toString();

      assert(fileContents == previousCommitContents);

      return Reset.reset(test.repo, test.currentCommit, Reset.TYPE.HARD);
    });
  });

  it("reset fails if parameter is not a Commit object", function() {
    var test = this;
    var commit = test.repo.getReferenceCommit("master");
    try {
      Reset.reset(test.repo, commit, Reset.TYPE.HARD);
      assert.fail(
        "Should not be able to pass a Promise (Commit) into the function"
      );
    } catch (err) {
      // ok
      assert.equal(
        "Repository and target commit's repository does not match",
        err.message
      );
    }
  });

  it("reset fails if originating repository is not the same", function() {
    var test = this;
    var testCommit = null;
    return test.repo.getReferenceCommit("master")
    .then(function(commit) {
      testCommit = commit;
      return Repository.open(reposPath);
    })
    .then(function(repo) {
      return Reset.reset(repo, testCommit, Reset.TYPE.HARD);
    })
    .then(function() {
      assert.fail("Different source repository instance should fail");
    })
    .catch(function(err) {
      assert.equal(
        "Repository and target commit's repository does not match",
        err.message
      );
    });
  });
});
