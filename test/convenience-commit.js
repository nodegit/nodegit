var git = require('../');
var rimraf = require('rimraf');
var fs = require( 'fs' );

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
 *
 * @param  {Object} test
 */
exports.method = function(test){
  test.expect(2);

  helper.testFunction(test.equals, git.commit, 'Commmit');

  test.done();
};

/**
 * Test that
 *
 * @param  {Object} test
 */
exports.improperCommitId = function(test) {
  test.expect(1);
  git.repo('../.git', function(error, repository) {

    repository.commit('not a proper commit sha', function(error, commit) {

      test.equals(error, 'Object does not exist in the scope searched.', 'Attempting to get commit by invalid SHA should error');

      test.done();
    });
  });
};

var historyCountKnownSHA = 'fce88902e66c72b5b93e75bdb5ae717038b221f6';

/**
 * Test that retreiving walking a given commit's history works as expected.
 *
 * @param  {Object} test
 */
exports.history = function(test) {
  test.expect(2);
  git.repo('../.git', function(error, repository) {

    repository.commit(historyCountKnownSHA, function(error, commit) {

      test.equals(error, 0, 'Getting latest branch commit should not error');

      var historyCount = 0;
      var expectedHistoryCount = 364;
      commit.history().on('commit', function(commit) {
        historyCount++;
      }).on('end', function(commits) {

        test.equals(historyCount, expectedHistoryCount, 'History count does not match expected');

        test.done();
      });
    });
  });
};

/**
 * Test that retreiving master branch's HEAD commit works as expected.
 *
 * @param  {Object} test
 */
exports.masterHead = function(test) {
  test.expect(2);
  git.repo('../.git', function(error, repository) {
    repository.branch('master', function(error, branch) {

      test.equals(error, 0, 'Getting branch should not error');

      repository.commit(branch.sha, function(error, commit) {

        test.equals(error, 0, 'Getting latest branch commit should not error');

        test.done();
      });
    });
  });
};

/**
 * Test that retrieving and walking a commit's tree works as expected.
 *
 * @param  {Object} test
 */
exports.tree = function(test) {
  test.expect(2);
  git.repo('../.git', function(error, repository) {

    repository.commit(historyCountKnownSHA, function(error, commit) {

      test.equals(error, 0, 'Getting latest branch commit should not error');

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
