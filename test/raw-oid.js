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

// Oid
exports.constructor = function(test){
  test.expect(3);

  // Test for function
  helper.testFunction(test.equals, git.Oid, 'Oid');

  // Ensure we get an instance of Oid
  test.ok(new git.Oid() instanceof git.Oid, 'Invocation returns an instance of Oid');

  test.done();
};

// Oid::FromString
exports.fromString = function(test) {
  test.expect(6);

  var testOid = new git.Oid();

  // Test for function
  helper.testFunction(test.equals, testOid.fromString, 'Oid::FromString');

  // Test path argument existence
  helper.testException(test.ok, function() {
    testOid.fromString();
  }, 'Throw an exception if no hex String');

  // Test that both arguments result correctly
  helper.testException(test.ifError, function() {
    testOid.fromString("somestr", function() {});
  }, 'No exception is thrown with proper arguments');

  // Test invalid hex id string
  testOid.fromString('1392DLFJIOS', function(error, oid) {
    test.notEqual(null, error, 'Invalid hex id String');
    testOid.fromString('1810DFF58D8A660512D4832E740F692884338CCD', function(error, oid) {
      // Test valid hex id string
      test.equal(null, error, 'Valid hex id String');
      test.done();
    });
  });
};

// Oid::Sha
exports.sha = function(test) {
  test.expect(3);
  var testOid = new git.Oid();

  // Test for function
  helper.testFunction(test.equals, testOid.sha, 'Oid::Sha');

  // Test valid hex id string
  var sha = '1810DFF58D8A660512D4832E740F692884338CCD';
  testOid.fromString(sha, function(error, rawOid) {
    test.equals(sha, testOid.sha().toUpperCase(), 'Valid hex id String');
    test.done();
  });
};
