var git = require( '../' ).raw,
    rimraf = require( 'rimraf' ),
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
exports.constructor = function(test){
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
exports.lookup = function(test) {
  var testOid = new git.Oid(),
      testCommit = new git.Commit();

  testOid.mkstr('cb09e99e91d41705197e0fb60823fdc7df776691');

  test.expect(8);

  // Test for function
  helper.testFunction(test.equals, testCommit.lookup, 'Commit::Lookup');

  // Test repo argument existence
  helper.testException( test.ok, function() {
    testCommit.lookup();
  }, 'Throw an exception if no repo' );

  // Test oid argument existence
  helper.testException(test.ok, function() {
    testCommit.lookup(testRepo);
  }, 'Throw an exception if no oid' );

  // Test callback argument existence
  helper.testException(test.ok, function() {
    testCommit.lookup(testOid);
  }, 'Throw an exception if no callback' );

  // Test that all arguments result correctly
  helper.testException(test.ifError, function() {
    testCommit.lookup(testRepo, testOid, function() {} );
  }, 'No exception is thrown with proper arguments' );

  testRepo.open( path.resolve('../.git'), function() {
    // Test invalid commit
    testOid.mkstr('100644');
    testCommit.lookup(testRepo, testOid, function(err) {
      test.notEqual(0, err, 'Not a valid commit');

      // Test valid commit
      testOid.mkstr('cb76e3c030ab29db332aff3b297dc39451a84762');
      testCommit.lookup( testRepo, testOid, function(err) {
        test.equals(null, err, 'Valid commit');

        //test.equals( 'Updated gitignore and raw-commit test', testCommit.messageShort(), 'Commit message is valid' );

        test.done();
      });
    });
  });
};

exports.fetchDetails = function(test) {

  test.expect(14);

  var testOid = new git.Oid();
  testOid.mkstr('cb76e3c030ab29db332aff3b297dc39451a84762');
  testRepo.open(path.resolve('../.git'), function() {
    var testCommit = new git.Commit();
    testCommit.lookup(testRepo, testOid, function(error, commit) {
      commit.fetchDetails(function(error, details) {

         var expected =  {
           sha: 'cb76e3c030ab29db332aff3b297dc39451a84762',
           message: 'bumped package.json up\n',
           time: 1300145116,
           timeOffset: -240,
           committer:
            { name: 'Tim Branyen',
                    email: 'tim.branyen@gmail.com',
              when: { when: 1300145116, offset: -240 } },
           author:
            { name: 'Tim Branyen',
                    email: 'tim.branyen@gmail.com',
              when: { when: 1300145116, offset: -240 } },
           parentCount: 1,
           parentShas: [ 'b1f941c62f508db5f392a6bb0ea1d591753a045b' ] };

        test.equals(expected.sha, details.sha, 'Expected SHA does not match result');
        test.equals(expected.message, details.message, 'Expected message does not match result');
        test.equals(expected.time, details.time, 'Expected time does not match result');
        test.equals(expected.offset, details.offset, 'Expected offset does not match result');

        test.equals(expected.committer.name, details.committer.name, 'Expected committer.name does not match result');
        test.equals(expected.committer.email, details.committer.email, 'Expected committer.email does not match result');
        test.equals(expected.committer.when.time, details.committer.when.time, 'Expected committer.when.time does not match result');
        test.equals(expected.committer.when.offset, details.committer.when.offset, 'Expected committer.when.offset does not match result');

        test.equals(expected.author.name, details.author.name, 'Expected author.name does not match result');
        test.equals(expected.author.email, details.author.email, 'Expected author.email does not match result');
        test.equals(expected.author.when.time, details.author.when.time, 'Expected author.when.time does not match result');
        test.equals(expected.author.when.offset, details.author.when.offset, 'Expected author.when.offset does not match result');

        test.equals(expected.parentCount, details.parentCount, 'Expected parentCount does not match result');
        test.equals(expected.parentShas[0], details.parentShas[0], 'Expected parentShas[0] does not match result');

        test.done();
      });
    });
  });
};
