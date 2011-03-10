var git = require( '../' ).raw,
    rimraf = require( '../vendor/rimraf' ) || require( 'rimraf' ),
    path = require( 'path' ),
    fs = require( 'fs' );

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

// Repo
exports.constructor = function( test ){
  test.expect( 3 );

  // Test for function
  helper.testFunction( test.equals, git.Repo, 'Repo' );
  
  // Ensure we get an instance of Repo
  test.ok( new git.Repo() instanceof git.Repo, 'Invocation returns an instance of Repo' );

  test.done();
};

// Repo::Open
exports.open = function( test ) {
  var testRepo = new git.Repo();

  test.expect( 6 );

  // Test for function
  helper.testFunction( test.equals, testRepo.open, 'Repo::Open' );

  // Test path argument existence
  helper.testException( test.ok, function() {
    testRepo.open();
  }, 'Throw an exception if no path' );
 
  // Test callback argument existence
  helper.testException( test.ok, function() {
    testRepo.open( "some/path" );
  }, 'Throw an exception if no callback' );

  // Test invalid repository
  testRepo.open( '/etc/hosts', function( err ) {
    test.equals( -8, err, 'Invalid repository error code' );

    // Test valid repository
    testRepo.open( path.resolve( '../.git' ), function( err ) {
      test.equals( 0, err, 'Valid repository error code' );

//      testRepo.free();

      test.done();
    });
  });
};

// TODO: Figure out how write unit tests for free
// Repo::Free
exports.free = function( test ) {
  var testRepo = new git.Repo();

  test.expect( 2 );

  // Test for function
  helper.testFunction( test.equals, testRepo.free, 'Repo::Free' );

  test.done();
};

// Repo::Init
exports.init = function( test ) {
  var testRepo = new git.Repo();

  test.expect( 7 );

  // Test for function
  helper.testFunction( test.equals, testRepo.init, 'Repo::Init' );

  // Test path argument existence
  helper.testException( test.ok, function() {
    testRepo.init();
  }, 'Throw an exception if no path' );
 
  // Test is_bare argument existence
  helper.testException( test.ok, function() {
    testRepo.init( "some/path" );
  }, 'Throw an exception if no is_bare' );

  // Test callback argument existence
  helper.testException( test.ok, function() {
    testRepo.init( "some/path", true );
  }, 'Throw an exception if no callback' );

  // Cleanup, remove test repo directory - if it exists
  rimraf( './test.git', function() {
    // Create bare repo and test for creation
    testRepo.init( './test.git', true, function( err, path, is_bare ) {
      test.equals( 0, err, 'Successfully created bare repository' );
      // Verify repo exists
      testRepo.open( './test.git', function(err, path) {
        test.equals( 0, err, 'Valid repository created' );

          testRepo.free();       

          // Cleanup, remove test repo directory
          rimraf( './test.git', function() {
            test.done();
          });
      });
    });
  });
};

// Repo::LookupRef
exports.lookupRef = function( test ) {
  var testRepo = new git.Repo(),
      master = new git.Ref(testRepo);

  test.expect( 5 );

  // Test for function
  helper.testFunction( test.equals, testRepo.lookupRef, 'Repo::LookupRef' );

  // Test ref argument existence
  helper.testException( test.ok, function() {
    testRepo.lookupRef();
  }, 'Throw an exception if no reference' );
 
  // Test name argument existence
  helper.testException( test.ok, function() {
    testRepo.lookupRef( master );
  }, 'Throw an exception if no name' );

  // Test callback argument existence
  helper.testException( test.ok, function() {
    testRepo.lookupRef( master, 'refs/heads/master' );
  }, 'Throw an exception if no callback' );

  // Cleanup, remove test repo directory - if it exists
  rimraf( './test.git', function() {
  //  // Create bare repo and test for creation
  //  testRepo.init( './test.git', true, function( err, path, is_bare ) {
  //    test.equals( 0, err, 'Successfully created bare repository' );
  //    // Verify repo exists
  //    testRepo.open( './test.git', function(err, path) {
  //      test.equals( 0, err, 'Valid repository created' );
  //      test.equals( true, is_bare, 'Returns valid is_bare value' );

  //        testRepo.free();       

  //        // Cleanup, remove test repo directory
  //        rimraf( './test.git', function() {
            test.done();
  //        });
  //    });
  //  });
  });
};
