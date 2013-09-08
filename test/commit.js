var git = require('../'),
    rimraf = require('rimraf'),
    fs = require( 'fs' );

var historyCountKnownSHA = 'fce88902e66c72b5b93e75bdb5ae717038b221f6';

exports.message = function(test) {
  test.expect(2);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      var message = commit.message();
      test.equals(error, null, 'There should be no error');
      test.equals(message, 'Update README.md', 'Message should match expected value');
      test.done();
    });
  });
};

exports.sha = function(test) {
  test.expect(2);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      var sha = commit.sha();
      test.equals(error, null, 'There should be no error');
      test.equals(sha, historyCountKnownSHA, 'SHA should match expected value');
      test.done();
    });
  });
};

exports.time = function(test) {
  test.expect(2);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      var time = commit.timeMs();
      test.equals(error, null, 'There should be no error');
      test.equals(time, 1362012884000, 'Time should match expected value');
      test.done();
    });
  });
};

exports.date = function(test) {
  test.expect(2);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      var date = commit.date();
      test.equals(error, null, 'There should be no error');
      test.equals(date.getTime(), 1362012884000, 'Date should match expected value');
      test.done();
    });
  });
};

exports.offset = function(test) {
  test.expect(2);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      var offset = commit.offset();
      test.equals(error, null, 'There should be no error');
      test.equals(offset, 780, 'Offset should match expected value');
      test.done();
    });
  });
};

exports.author = function(test) {
  test.expect(2);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      var author = commit.author();
      test.equals(error, null, 'There should be no error');
      test.notEqual(author, null, 'Author should not be null');
      test.done();
    });
  });
};

exports.authorName = function(test) {
  test.expect(1);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      var author = commit.author();
      var name = author.name();
      test.equals(name, 'Michael Robinson', 'The author name should match expected value');
      test.done();
    });
  });
};

exports.authorEmail = function(test) {
  test.expect(1);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      var author = commit.author();
      var email = author.email();
      test.equals(email, 'mike@panmedia.co.nz', 'The author email should match expected value');
      test.done();
    });
  });
};

exports.committerName = function(test) {
  test.expect(1);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      var committer = commit.committer();
      var name = committer.name();
      test.equals(name, 'Michael Robinson', 'The author name should match expected value');
      test.done();
    });
  });
};

exports.committerEmail = function(test) {
  test.expect(1);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      var committer = commit.committer();
      var email = committer.email();
      test.equals(email, 'mike@panmedia.co.nz', 'The committer email should match expected value');
      test.done();
    });
  });
};

/**
 * Test that improper commit ID's result in an error message
 */
exports.improperCommitId = function(test) {
  test.expect(1);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit('not a proper commit sha', function(error, commit) {
      test.notEqual(error, null, 'Error should occur');
      test.done();
    });
  });
};

/**
 * Test that retreiving walking a given commit's history works as expected.
 */
exports.history = function(test) {
  test.expect(4);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      test.equals(null, error, 'Getting latest branch commit should not error');
      var historyCount = 0;
      var expectedHistoryCount = 364;
      commit.history().on('commit', function(commit) {
        historyCount++;
      }).on('end', function(commits) {
        test.equals(null, error, 'There should be no errors');
        test.equals(historyCount, expectedHistoryCount);
        test.equals(commits.length, expectedHistoryCount);
        test.done();
      }).on('error', function(error) {
        test.equals(null, error, 'There should be no errors');
        test.ok(false, 'There should be no errors');
      }).start();
    });
  });
};

/**
 * Test that retreiving master branch's HEAD commit works as expected.
 */
exports.masterHead = function(test) {
  test.expect(1);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getBranch('master', function(error, branch) {
      var sha = branch.sha();
      repository.getCommit(sha, function(error, commit) {
        test.equals(error, null, 'Getting latest branch commit should not error');
        test.done();
      });
    });
  });
};

/**
 * Test that retreiving parent works as expected.
 *
 * @param  {Object} test
 */
exports.parents = function(test) {
  test.expect(3);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      commit.getParents(function(error, parents) {
        test.equals(parents.length, 1, 'Commit should have exactly one parent');
        var sha = parents[0].sha();
        test.equals(error, null, 'Getting parent SHA should not error');
        test.equals(sha, 'ecfd36c80a3e9081f200dfda2391acadb56dac27', 'Parent SHA should match expected value');
        test.done();
      });
    });
  });
};

/**
 * Test that retrieving and walking a commit's tree works as expected.
 */
exports.tree = function(test) {
  test.expect(2);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      test.equals(error, null, 'Getting latest branch commit should not error');

      var commitTreeEntryCount = 0;
      var expectedCommitTreeEntryCount = 198;
      commit.getTree(function(error, tree) {
        tree.walk().on('entry', function(entry) {
          commitTreeEntryCount++;
        }).on('end', function(error, entries) {
          test.equals(commitTreeEntryCount, expectedCommitTreeEntryCount, 'Commit tree entry count does not match expected');
          test.done();
        }).start();
      });
    });
  });
};

/**
 * Test that getDiff works as expected.
 */
exports.getDiff = function(test) {
  test.expect(1);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      commit.getDiff(function(error, diff) {
        test.equals(diff.length, 1, 'Should be one item in parents diff trees');
        test.done();
      });
    });
  });
};

process.on('uncaughtException', function(err) {
  console.log(err.stack);
});

