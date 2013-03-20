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
    } catch (ex) {
      test(true, label);
    }
  }
};

/**
 * Repo
 * Ensure the repo method can handle opening repositories with async/sync
 * signatures properly.
 */
exports.method = function(test){
  test.expect(5);

  helper.testFunction(test.equals, git.repo, 'Repo');

  // Test callback argument existence
  helper.testException(test.ok, function() {
    git.repo('some/path');
  }, 'Throw an exception if no callback');

  // Test invalid repository
  git.repo('/etc/hosts', function(error, path) {
    test.equals(error.code, error.GITERR_REPOSITORY, error.message, 'Invalid repository error code');

    // Test valid repository
    git.repo('../.git', function(error, path) {
      test.equals(null, error, 'Valid repository error code');
      test.done();
    });
  });
};

/**
 * Ensure repo doesn't attempt to open missing directories
 */
exports.nonexistentDirectory = function(test) {
    test.expect(2);
    git.repo('/surely/this/directory/does/not/exist/on/this/machine', function(error, repository) {
        test.notEqual(error, null, 'Attempting to open a nonexistent directory should error');
        test.equals(repository, null, 'Non existent directory should result in null repository');
        test.done();
    });
};

/**
 * Repo::Init
 *
 * Ensure the init method can create repositories at the destination path and
 * can create either bare/non-bare.  This should work async/sync and provide
 * the proper return values.
 */
exports.init = function(test) {
  test.expect(4);

  helper.testFunction(test.equals, git.repo().init, 'Repo::Init');

  // Cleanup, remove test repo directory - if it exists
  rimraf('./test.git', function() {
    // Create bare repo and test for creation
    git.repo().init('./test.git', true, function(error, path, isBare) {
      test.equals(null, error, 'Successfully created bare repository');
      // Verify repo exists
      git.repo('./test.git', function(error, path, repo) {
        test.equals(null, error, 'Valid repository created');

        // Cleanup, remove test repo directory
        rimraf('./test.git', test.done);
      });
    });
  });
};
