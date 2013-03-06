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
 * Test that the error object is present.
 *
 * @param  {Object} test
 */
exports.method = function(test){
  test.expect(2);

  helper.testFunction(test.equals, git.error, 'Error');

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

      test.notEqual(error.code, git.error.GIT_SUCCESS, 'Attempting to get commit by invalid SHA should error');

      test.done();
    });
  });
};

