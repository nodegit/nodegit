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
 * Test that the error object is present.
 *
 * @param  {Object} test
 */
exports.method = function(test){
  test.expect(2);
  helper.testFunction(test.equals, git.error, 'Error');
  test.done();
};

exports.codes = function(test) {
  test.expect(14);
  var error = new git.error();
  test.equal(error.codes.GITERR_NOMEMORY, git.raw.Error.codes.GITERR_NOMEMORY, 'GITERR_NOMEMORY code should match expected value');
  test.equal(error.codes.GITERR_OS, git.raw.Error.codes.GITERR_OS, 'GITERR_OS code should match expected value');
  test.equal(error.codes.GITERR_INVALID, git.raw.Error.codes.GITERR_INVALID, 'GITERR_INVALID code should match expected value');
  test.equal(error.codes.GITERR_REFERENCE, git.raw.Error.codes.GITERR_REFERENCE, 'GITERR_REFERENCE code should match expected value');
  test.equal(error.codes.GITERR_ZLIB, git.raw.Error.codes.GITERR_ZLIB, 'GITERR_ZLIB code should match expected value');
  test.equal(error.codes.GITERR_REPOSITORY, git.raw.Error.codes.GITERR_REPOSITORY, 'GITERR_REPOSITORY code should match expected value');
  test.equal(error.codes.GITERR_CONFIG, git.raw.Error.codes.GITERR_CONFIG, 'GITERR_CONFIG code should match expected value');
  test.equal(error.codes.GITERR_REGEX, git.raw.Error.codes.GITERR_REGEX, 'GITERR_REGEX code should match expected value');
  test.equal(error.codes.GITERR_ODB, git.raw.Error.codes.GITERR_ODB, 'GITERR_ODB code should match expected value');
  test.equal(error.codes.GITERR_INDEX, git.raw.Error.codes.GITERR_INDEX, 'GITERR_INDEX code should match expected value');
  test.equal(error.codes.GITERR_OBJECT, git.raw.Error.codes.GITERR_OBJECT, 'GITERR_OBJECT code should match expected value');
  test.equal(error.codes.GITERR_NET, git.raw.Error.codes.GITERR_NET, 'GITERR_NET code should match expected value');
  test.equal(error.codes.GITERR_TAG, git.raw.Error.codes.GITERR_TAG, 'GITERR_TAG code should match expected value');
  test.equal(error.codes.GITERR_TREE, git.raw.Error.codes.GITERR_TREE, 'GITERR_TREE code should match expected value');
  test.done();
};

exports.returnCodes = function(test) {
  test.expect(8);
  var error = new git.error();
  test.equal(error.returnCodes.GIT_OK, git.raw.Error.returnCodes.GIT_OK, 'GIT_OK return code should match expected value');
  test.equal(error.returnCodes.GIT_ERROR, git.raw.Error.returnCodes.GIT_ERROR, 'GIT_ERROR return code should match expected value');
  test.equal(error.returnCodes.GIT_ENOTFOUND, git.raw.Error.returnCodes.GIT_ENOTFOUND, 'GIT_ENOTFOUND return code should match expected value');
  test.equal(error.returnCodes.GIT_EEXISTS, git.raw.Error.returnCodes.GIT_EEXISTS, 'GIT_EEXISTS return code should match expected value');
  test.equal(error.returnCodes.GIT_EAMBIGUOUS, git.raw.Error.returnCodes.GIT_EAMBIGUOUS, 'GIT_EAMBIGUOUS return code should match expected value');
  test.equal(error.returnCodes.GIT_EBUFS, git.raw.Error.returnCodes.GIT_EBUFS, 'GIT_EBUFS return code should match expected value');
  test.equal(error.returnCodes.GIT_PASSTHROUGH, git.raw.Error.returnCodes.GIT_PASSTHROUGH, 'GIT_PASSTHROUGH return code should match expected value');
  test.equal(error.returnCodes.GIT_ITEROVER, git.raw.Error.returnCodes.GIT_ITEROVER, 'GIT_ITEROVER return code should match expected value');
  test.done();
};

/**
 * Test that
 *
 * @param  {Object} test
 */
exports.improperCommitId = function(test) {
  test.expect(1);
  git.repo.open('../.git', function(error, repository) {
    repository.commit('not a proper commit sha', function(error, commit) {
      test.notEqual(error.code, git.error.GIT_SUCCESS, 'Attempting to get commit by invalid SHA should error');
      test.done();
    });
  });
};

