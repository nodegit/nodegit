var git = require('../'),
    rimraf = require('rimraf'),
    fs = require( 'fs' );

/**
 * Repo
 * Ensure the repo method can handle opening repositories with async/sync
 * signatures properly.
 */
exports.open = function(test){
  test.expect(2);

  // Test invalid repository
  git.Repo.open('/private/etc/hosts', function(error, repository) {
    test.equals(error.message, "The `.git` file at '/private/etc/hosts' is malformed");

    // Test valid repository
    git.Repo.open('../.git', function(error, repository) {
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
  git.Repo.open('/surely/this/directory/does/not/exist/on/this/machine', function(error, repository) {
    test.notEqual(error, null, 'Attempting to open a nonexistent directory should error');
    test.equals(repository, null, 'Non existent directory should result in null repository');
    test.done();
  });
};

/**
 * Ensure the init method can create repositories at the destination path and
 * can create either bare/non-bare.
 */
exports.init = function(test) {
  test.expect(2);
  // Cleanup, remove test repo directory - if it exists
  rimraf('./test.git', function() {
    // Create bare repo and test for creation
    git.Repo.init('./test.git', true, function(error, path, isBare) {
      test.equals(null, error, 'Successfully created bare repository');
      // Verify repo exists
      git.Repo.open('./test.git', function(error, path, repo) {
        test.equals(null, error, 'Valid repository created');

        // Cleanup, remove test repo directory
        rimraf('./test.git', test.done);
      });
    });
  });
};
