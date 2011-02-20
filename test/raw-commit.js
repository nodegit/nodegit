var git = require( 'nodegit2' ).git2,
    rimraf = require( '../vendor/rimraf');

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

// Oid
exports.constructor = function( test ){
  test.expect( 3 );

  // Test for function
  helper.testFunction( test.equals, git.Commit, 'Commit' );
  
  // Ensure we get an instance of Oid
  test.ok( new git.Commit() instanceof git.Commit, 'Invocation returns an instance of Commit' );

  test.done();
};

// Oid::Mkstr
exports.lookup = function( test ) {
  var testOid = new git.Oid(),
      testCommit = new git.Commit();

  testOid.mkstr( '978feacee2432e67051f2714ec7d28ad80e16908' );

  test.expect( 11 );

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


  testRepo.open( './dummyrepo/.git', function( err, path ) {
    // Test invalid commit
    testOid.mkstr( '100644' );
    testCommit.lookup( testRepo, testOid, function( err, details ) {
      test.notEqual( 0, err, 'Not a valid commit' );
 
      // Test valid commit
      testOid.mkstr( '978feacee2432e67051f2714ec7d28ad80e16908' );
      testCommit.lookup( testRepo, testOid, function( err, details ) {
        test.equals( 0, err, 'Valid commit');

        test.equals( 'object', typeof details, 'Details is an object' );

        test.equals( 'string', typeof details.message, 'Details message is a String' );
        if(details.message) {
          test.equals( 'initial commit', details.message.trim(), 'Details has correct message' );
        }

        testRepo.free();

        test.done();
      });
    });
  });
};
