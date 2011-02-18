var git = require( '../build/default/git2' );

// Helper function
function testException( test, fun, message ) {
  try {
    fun();
    test( false, message );
  }
  catch (ex) {
    test( true, message );
  }
}

// Constructor tests
exports.constructor = function( test ){
  test.expect( 3 );
  
  // The object reports itself as a function
  test.equals( typeof git.Repo, 'function', 'Repo reports as a function.' );
  // This ensures the repo is actually a derivative of the Function [[Class]]
  test.equals( toString.call( git.Repo ), '[object Function]', 'Repo [[Class]] is of type function.' );
  // Ensure we get an instance of Repo
  test.ok( new git.Repo() instanceof git.Repo, 'Invocation returns an instance of Repo' );



  test.done();
};

// Open method
exports.open = function( test ) {
  var testRepo = new git.Repo();

  test.expect( 6 );

  // Test path argument existence
  testException( test.ok, function() {
    testRepo.open();
  }, 'Throw an exception if no path' );
 
  // Test callback argument existence
  testException( test.ok, function() {
    testRepo.open( "some/path" );
  }, 'Throw an exception if no callback' );

  // Test that both arguments result correctly
  testException( test.ifError, function() {
    testRepo.open( "some/path", function() {} );
  }, 'No exception is thrown which proper arguments' );

  // Test invalid repository
  testRepo.open( '/etc/hosts', function( err, path ) {
    test.equals( -8, err, 'Invalid repository error code' );

    // Test valid repository
    testRepo.open( './.git', function( err, path ) {
      test.equals( 0, err, 'Valid repository error code' );

      // Test path returned is correct
      test.equals( './.git', path, 'Path return matches sent' );

      test.done();
    });
  });
};
