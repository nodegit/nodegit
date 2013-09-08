var git = require('../'),
    rimraf = require('rimraf'),
    fs = require( 'fs' );

/**
 * Repo
 * Ensure the repo method can handle opening repositories with async/sync
 * signatures properly.
 */
exports.openInvalidRepo = function(test){
  test.expect(1);

  // Test invalid repository
  git.Repo.open('repos/nonrepo', function(error, repository) {
    test.equals(error.message, "Could not find repository from 'repos/nonrepo'");
    test.done();
  });
};

exports.openValidRepo = function(test){
  test.expect(1);
  
  // Test valid repository
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    test.equals(null, error, 'Valid repository error code');
    test.done();
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
  // Create bare repo and test for creation
  git.Repo.init('repos/newrepo', true, function(error, path, isBare) {
    test.equals(null, error, 'Successfully created bare repository');
    // Verify repo exists
    git.Repo.open('repos/newrepo', function(error, path, repo) {
      test.equals(null, error, 'Valid repository created');
      test.done();
    });
  });
};
