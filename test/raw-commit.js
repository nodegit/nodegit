var git = require('../').raw;

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

/**
 * Commit::Lookup
 */
exports.lookup = function(test) {
  test.expect(3);

  var testOid = git.Oid.fromString('cb76e3c030ab29db332aff3b297dc39451a84762');

  git.Repo.open('../.git', function(error, repo) {
    // Test oid argument existence
    helper.testException(test.ok, function() {
      repo.getCommit();
    }, 'Throw an exception if no oid');

    // Test that all arguments result correctly
    helper.testException(test.ifError, function() {
      repo.getCommit(testOid, function() {});
    }, 'No exception is thrown with proper arguments');
    // Test valid commit
    repo.getCommit(testOid, function(err) {
      test.equal(null, err, 'Valid commit');
      test.done();
    });
  });
};
