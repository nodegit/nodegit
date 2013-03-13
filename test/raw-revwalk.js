var git = require( '../' ).raw,
    path = require('path'),
    rimraf = require('rimraf');

var testRepo = new git.Repo();

// Helper functions
var helper = {
  // Test if obj is a true function
  testFunction: function( test, obj, label ) {
    // The object reports itself as a function
    test( typeof obj, 'function', label +' reports as a function.' );
    // This ensures the repo is actually a derivative of the Function [[Class]]
    test( toString.call( obj ), '[object Function]', label +' [[Class]] is of type function.' );
  },
  // Test code and handle exception thrown
  testException: function( test, fun, label ) {
    try {
      fun();
      test( false, label );
    }
    catch (ex) {
      test( true, label );
    }
  }
};

/**
 * RevWalk
 */
exports.constructor = function(test){
  test.expect(3);

  // Test for function
  helper.testFunction(test.equals, git.RevWalk, 'RevWalk');

  // Ensure we get an instance of Oid
  testRepo.open('../.git', function(error, repository) {
    test.ok(new git.RevWalk(repository) instanceof git.RevWalk, 'Invocation returns an instance of RevWalk');
    test.done();
  });
};
