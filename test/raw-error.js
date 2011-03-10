var git = require( '../' ).raw,
    rimraf = require( '../vendor/rimraf' ) || require( 'rimraf' );

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

// Error
exports.constructor = function( test ){
  test.expect( 3 );

  // Test for function
  helper.testFunction( test.equals, git.Error, 'Error' );
  
  // Ensure we get an instance of Error
  test.ok( new git.Error() instanceof git.Error, 'Invocation returns an instance of Error' );

  test.done();
};

// Error::StrError
exports.str_error = function( test ) {
  var testError = new git.Error();

  test.expect( 6 );

  // Test for function
  helper.testFunction( test.equals, testError.strError, 'Error::StrError' );

  // Test path argument existence
  helper.testException( test.ok, function() {
    testError.strError();
  }, 'Throw an exception if no error code' );
 
  // Test that arguments result correctly
  helper.testException( test.ifError, function() {
    testError.strError( 0 );
  }, 'No exception is thrown with proper arguments' );

  // Finding an actual error
  test.equals( 'Input was not a properly formatted Git object id.', testError.strError( -2 ), 'Returns proper error message per code.' );

  // Returning a consistent error message for all unknown errors
  test.equals( 'Unknown error', testError.strError( 1000 ), 'Returns consistent error message with an unknown.' );

  test.done();
};
