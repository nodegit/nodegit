var git = require('../'),
    rimraf = require('rimraf'),
    fs = require( 'fs' );

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

/**
 * Test that improper commit ID's result in an error message
 */
exports.improperCommitId = function(test) {
  test.expect(1);
  git.repo('../.git', function(error, repository) {
    repository.commit('not a proper commit sha', function(error, commit) {
      test.notEqual(error, null, 'Error should occur');
      test.done();
    });
  });
};

var historyCountKnownSHA = 'fce88902e66c72b5b93e75bdb5ae717038b221f6';

/**
 * Test that retreiving walking a given commit's history works as expected.
 */
exports.history = function(test) {
  test.expect(368);
  git.repo('../.git', function(error, repository) {
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
  git.repo('../.git', function(error, repository) {
    repository.branch('master', function(error, branch) {

      test.equals(error, null, 'Getting branch should not error');

      repository.commit(branch.sha, function(error, commit) {

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
exports.parentSync = function(test) {
  test.expect(2);
  git.repo('../.git', function(error, repository) {
    repository.commit('2d71044741412280370cb0326c96d3a5a7b5dca1', function(error, commit) {
      test.equals(commit.parentCount, 1, 'Commit has exactly one parent');
      var parent = commit.parentSync(0);
      test.equals(parent.sha, 'e8876707938abf94d5cc02b0c4017c4fec2aa44e', 'Parent SHA should match expected value');
      test.done();
    });
  });
};

/**
 * Test that retreiving parent works as expected.
 *
 * @param  {Object} test
 */
exports.parent = function(test) {
  test.expect(2);
  git.repo('../.git', function(error, repository) {
    repository.commit('2d71044741412280370cb0326c96d3a5a7b5dca1', function(error, commit) {
      test.equals(commit.parentCount, 1, 'Commit has exactly one parent');
      commit.parent(0, function(error, parent) {
        if (error) throw error;
        test.equals(parent.sha, 'e8876707938abf94d5cc02b0c4017c4fec2aa44e', 'Parent SHA should match expected value');
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
  git.repo('../.git', function(error, repository) {

    repository.commit(historyCountKnownSHA, function(error, commit) {

      test.equals(error, null, 'Getting latest branch commit should not error');

      var commitTreeEntryCount = 0;
      var expectedCommitTreeEntryCount = 200;

      commit.tree().walk().on('entry', function(commit) {
        commitTreeEntryCount++;
      }).on('end', function(commits) {

        test.equals(commitTreeEntryCount, expectedCommitTreeEntryCount, 'Commit tree entry count does not match expected');

        test.done();
      });
    });
  });
};

/**
 * Test that parentsDiffTrees works as expected.
 */
exports.parentsDiffTrees = function(test) {
  test.expect(1);
  git.repo('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      commit.parentsDiffTrees(function(error, parentsDiffTrees) {
        test.equals(parentsDiffTrees.length, 1, 'Should be one item in parents diff trees');
        test.done();
      });
    });
  });
};

exports.file = function(test) {
  test.expect(5);
  git.repo('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      commit.file('README.md', function(error, file) {
        test.equal(error, null, 'Should not error');
        test.notEqual(file, null, 'File should not be null');
        test.equal(file.name, 'README.md', 'File name should match expected');
        test.equal(file.sha, 'b252f396b17661462372f78b7bcfc403b8731aaa', 'SHA shoud match expected');
        test.equal(file.attributes, 33188, 'Attributes should match expected');
        test.done();
      });
    });
  });
};
