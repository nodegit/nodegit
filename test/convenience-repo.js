var git = require( '../' ),
    rimraf = require( '../vendor/rimraf') || require( 'rimraf' ),
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
  test.expect( 6 );

  // Test for function
  helper.testFunction( test.equals, git.repo, 'Repo' );

  // Test callback argument existence
  helper.testException( test.ok, function() {
    git.repo( 'some/path' );
  }, 'Throw an exception if no callback' );

  // Test invalid repository
  git.repo( '/etc/hosts', function( err, path ) {
    test.equals( 'The specified repository is invalid', err, 'Invalid repository error code' );

    // Test valid repository
    git.repo( './.git', function( err, path ) {
      test.equals( 0, err, 'Valid repository error code' );

      // Test path returned is correct
      test.equals( './.git', path, 'Path return matches sent' );

      test.done();
    });
  });
};

// Repo::Init
exports.init = function( test ) {
  test.expect( 5 );

  // Test for function
  helper.testFunction( test.equals, git.repo().init, 'Repo::Init' );

  // Test path argument existence
  //helper.testException( test.ok, function() {
  //  git.repo().init();
  //}, 'Throw an exception if no path' );
 
  //// Test is_bare argument existence
  //helper.testException( test.ok, function() {
  //  git.repo().init( 'some/path' );
  //}, 'Throw an exception if no is_bare' );

  // Cleanup, remove test repo directory - if it exists
  rimraf( './test.git', function() {
    // Create bare repo and test for creation
    git.repo().init( './test.git', true, function( err, path, is_bare ) {
      test.equals( 0, err, 'Successfully created bare repository' );
      // Verify repo exists
      git.repo('./test.git', function(err, path, repo) {
        test.equals( 0, err, 'Valid repository created' );
        test.equals( true, is_bare, 'Returns valid is_bare value' );

        // Test repo branch lookup
        repo.branch( 'master', function( err ) {
          console.log(err);
          test.done();

          // Cleanup, remove test repo directory
          rimraf( './test.git', function() {

          });
        });
      });
    });
  });
};
