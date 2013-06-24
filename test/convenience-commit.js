var git = require('../'),
    rimraf = require('rimraf'),
    fs = require( 'fs' );

var historyCountKnownSHA = 'fce88902e66c72b5b93e75bdb5ae717038b221f6';

// Helper functions
var helper = {
  // Test if obj is a true function
  testFunction: function(test, obj, label) {
    // The object reports itself as a function
    test(typeof obj, 'function', label + ' reports as a function.');
    // This ensures the repo is actually a derivative of the Function [[Class]]
    test(toString.call(obj), '[object Function]', label + ' [[Class]] is of type function.');
  },
  // Test code and handle exception thrown
  testException: function(test, fun, label) {
    try {
      fun();
      test(false, label);
    }
    catch (ex) {
      test(true, label);
    }
  }
};

/**
 * Test that the commit object is present.
 */
exports.method = function(test){
  test.expect(2);
  helper.testFunction(test.equals, git.commit, 'Commmit');
  test.done();
};

exports.message = function(test) {
  test.expect(3);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      var message = commit.message();
      test.equals(error, null, 'There should be no error');
      test.notEqual(message, null, 'Message should not be null');
      test.equals(message, 'Update README.md', 'Message should match expected value');
      test.done();
    });
  });
};

exports.sha = function(test) {
  test.expect(3);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      var sha = commit.sha();
      test.equals(error, null, 'There should be no error');
      test.notEqual(sha, null, 'SHA should not be null');
      test.equals(sha, historyCountKnownSHA, 'SHA should match expected value');
      test.done();
    });
  });
};

exports.time = function(test) {
  test.expect(3);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      var time = commit.time();
      test.equals(error, null, 'There should be no error');
      test.notEqual(time, null, 'Time should not be null');
      test.equals(time, 1362012884000, 'Time should match expected value');
      test.done();
    });
  });
};

exports.date = function(test) {
  test.expect(4);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      var date = commit.date();
      test.equals(error, null, 'There should be no error');
      test.notEqual(date, null, 'Date should not be null');
      test.equal(date instanceof Date, true, 'Date should be a date object');
      test.equals(date.getTime(), 1362012884000, 'Date should match expected value');
      test.done();
    });
  });
};

exports.offset = function(test) {
  test.expect(3);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      var offset = commit.offset();
      test.equals(error, null, 'There should be no error');
      test.notEqual(offset, null, 'Offset should not be null');
      test.equals(offset, 780, 'Offset should match expected value');
      test.done();
    });
  });
};

exports.author = function(test) {
  test.expect(2);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      var author = commit.author();
      test.equals(error, null, 'There should be no error');
      test.notEqual(author, null, 'Author should not be null');
      test.done();
    });
  });
};

exports.authorName = function(test) {
  test.expect(1);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      var author = commit.author();
      var name = author.name();
      test.equals(name, 'Michael Robinson', 'The author name should match expected value');
      test.done();
    });
  });
};

exports.authorEmail = function(test) {
  test.expect(1);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      var author = commit.author();
      var email = author.email();
      test.equals(email, 'mike@panmedia.co.nz', 'The author email should match expected value');
      test.done();
    });
  });
};

exports.committerName = function(test) {
  test.expect(1);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      var committer = commit.committer();
      var name = committer.name();
      test.equals(name, 'Michael Robinson', 'The author name should match expected value');
      test.done();
    });
  });
};

exports.committerEmail = function(test) {
  test.expect(1);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
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
  git.repo.open('../.git', function(error, repository) {
    repository.commit('not a proper commit sha', function(error, commit) {
      test.notEqual(error, null, 'Error should occur');
      test.done();
    });
  });
};

/**
 * Test that retreiving walking a given commit's history works as expected.
 */
exports.history = function(test) {
  test.expect(368);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      test.equals(null, error, 'Getting latest branch commit should not error');
      var historyCount = 0;
      var expectedHistoryCount = 364;
      commit.history().on('commit', function(error, commit) {
        test.equals(null, error, 'There should be no errors');
        historyCount++;
      }).on('end', function(error, commits) {
        test.equals(null, error, 'There should be no errors');
        test.equals(historyCount, expectedHistoryCount, 'Manual count does not match expected');
        test.equals(commits.length, expectedHistoryCount, '"end" count does not match expected');
        test.done();
      });
    });
  });
};

/**
 * Test that retreiving master branch's HEAD commit works as expected.
 */
exports.masterHead = function(test) {
  test.expect(2);
  git.repo.open('../.git', function(error, repository) {
    repository.branch('master', function(error, branch) {
      test.equals(error, null, 'Getting branch should not error');
      var sha = branch.sha();
      repository.commit(sha, function(error, commit) {
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
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      commit.parents(function(error, parents) {
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
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
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
 * Test that parentsDiffTrees works as expected.
 */
exports.parentsDiffTrees = function(test) {
  test.expect(1);
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      commit.parentsDiffTrees(function(error, parentsDiffTrees) {
        test.equals(parentsDiffTrees.length, 1, 'Should be one item in parents diff trees');
        test.done();
      });
    });
  });
};

process.on('uncaughtException', function(err) {
  console.log(err);
});

