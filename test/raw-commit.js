var git = require( '../' ).raw,
    rimraf = require( '../vendor/rimraf' ) || require( 'rimraf' ),
    path = require( 'path' );

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

// Commit
exports.constructor = function( test ){
  test.expect( 3 );

  // Test for function
  helper.testFunction( test.equals, git.Commit, 'Commit' );
  
  testRepo.open( path.resolve( '../.git' ), function( err ) {
    // Ensure we get an instance of Commit
    test.ok( new git.Commit( testRepo ) instanceof git.Commit, 'Invocation returns an instance of Commit' );

    test.done();
  });
};

// Commit::Lookup
exports.lookup = function( test ) {
  var testOid = new git.Oid(),
      testCommit = new git.Commit( testRepo );

  testOid.mkstr( 'cb09e99e91d41705197e0fb60823fdc7df776691' );

  test.expect( 9 );

  // Test for function
  helper.testFunction( test.equals, testCommit.lookup, 'Commit::Lookup' );

  // Test repo argument existence
  helper.testException( test.ok, function() {
    testCommit.lookup();
  }, 'Throw an exception if no repo' );
 
  // Test oid argument existence
  helper.testException( test.ok, function() {
    testCommit.lookup( testRepo );
  }, 'Throw an exception if no oid' );

  // Test callback argument existence
  helper.testException( test.ok, function() {
    testCommit.lookup( testRepo, testOid );
  }, 'Throw an exception if no callback' );

  // Test that both arguments result correctly
  helper.testException( test.ifError, function() {
    testCommit.lookup( testRepo, testOid, function() {} );
  }, 'No exception is thrown with proper arguments' );

  testRepo.open( path.resolve( '../.git' ), function( err ) {
    console.log( new git.Error().strError( err ) );
    // Test invalid commit
    testOid.mkstr( '100644' );
    testCommit.lookup( testRepo, testOid, function( err ) {
      test.notEqual( 0, err, 'Not a valid commit' );
 
      // Test valid commit
      testOid.mkstr( '3b7670f327dc1ca66e040f0c09cc4c3f1428eb49' );
      testCommit.lookup( testRepo, testOid, function( err ) {
        test.equals( 0, err, 'Valid commit');

        test.equals( 'Fixed path issues', testCommit.messageShort(), 'Commit message is valid' );

        testRepo.free();

        test.done();
      });
    });
  });
};
