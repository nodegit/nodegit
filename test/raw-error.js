var git = require('../').raw,
    rimraf = require('rimraf');

// Helper functions
var helper = {
  // Test if obj is a true function
  testFunction: function(test, obj, label) {
    // The object reports itself as a function
    test(typeof obj, 'function', label +' reports as a function.');
    // This ensures the repo is actually a derivative of the Function [[Class]]
    test(toString.call(obj), '[object Function]', label +' [[Class]] is of type function.');
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

// Error
exports.constructor = function(test){
  test.expect(3);

  // Test for function
  helper.testFunction(test.equals, git.Error, 'Error');

  // Ensure we get an instance of Error
  test.ok(new git.Error() instanceof git.Error, 'Invocation returns an instance of Error');

  test.done();
};

exports.codes = function(test) {
  test.expect(23);

  test.equals(git.Error.returnCodes.GIT_OK, 0, 'GIT_OK should equal 0'),
  test.equals(git.Error.returnCodes.GIT_ERROR, -1, 'GIT_ERROR should equal  -1'),
  test.equals(git.Error.returnCodes.GIT_ENOTFOUND, -3, 'GIT_ENOTFOUND should equal  -3'),
  test.equals(git.Error.returnCodes.GIT_EEXISTS, -4, 'GIT_EEXISTS should equal  -4'),
  test.equals(git.Error.returnCodes.GIT_EAMBIGUOUS, -5, 'GIT_EAMBIGUOUS should equal  -5'),
  test.equals(git.Error.returnCodes.GIT_EBUFS, -6, 'GIT_EBUFS should equal  -6'),
  test.equals(git.Error.returnCodes.GIT_PASSTHROUGH, -30, 'GIT_PASSTHROUGH should equal -30'),
  test.equals(git.Error.returnCodes.GIT_ITEROVER, -31, 'GIT_ITEROVER should equal -31'),

  test.equals(git.Error.codes.GITERR_NOMEMORY, 0, 'GITERR_NOMEMORY should equal 0');
  test.equals(git.Error.codes.GITERR_OS, 1, 'GITERR_OS should equal 1');
  test.equals(git.Error.codes.GITERR_INVALID, 2, 'GITERR_INVALID should equal 2');
  test.equals(git.Error.codes.GITERR_REFERENCE, 3, 'GITERR_REFERENCE should equal 3');
  test.equals(git.Error.codes.GITERR_ZLIB, 4, 'GITERR_ZLIB should equal 4');
  test.equals(git.Error.codes.GITERR_REPOSITORY, 5, 'GITERR_REPOSITORY should equal 5');
  test.equals(git.Error.codes.GITERR_CONFIG, 6, 'GITERR_CONFIG should equal 6');
  test.equals(git.Error.codes.GITERR_REGEX, 7, 'GITERR_REGEX should equal 7');
  test.equals(git.Error.codes.GITERR_ODB, 8, 'GITERR_ODB should equal 8');
  test.equals(git.Error.codes.GITERR_INDEX, 9, 'GITERR_INDEX should equal 9');
  test.equals(git.Error.codes.GITERR_OBJECT, 10, 'GITERR_OBJECT should equal 10');
  test.equals(git.Error.codes.GITERR_NET, 11, 'GITERR_NET should equal 11');
  test.equals(git.Error.codes.GITERR_TAG, 12, 'GITERR_TAG should equal 12');
  test.equals(git.Error.codes.GITERR_TREE, 13, 'GITERR_TREE should equal 13');
  test.equals(git.Error.codes.GITERR_INDEXER, 14, 'GITERR_INDEXER should equal 14');

  test.done();
};
