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
  test.throws(function() { new git.Oid(); }, 'Cannot instantiate an Oid directly');

  test.done();
};

// Oid::FromString
exports.fromString = function(test) {
  test.expect(6);

  // Test for function
  helper.testFunction(test.equals, git.Oid.fromString, 'Oid::FromString');

  // Test path argument existence
  helper.testException(test.ok, function() {
    git.Oid.fromString();
  }, 'Throw an exception if no hex String');

  // Test that both arguments result correctly
  test.throws(function() { git.Oid.fromString("somestr", function() {}); });

  // Test valid hex id string
  test.throws(function() { git.Oid.fromString('1392DLFJIOS'); });
  test.doesNotThrow(function() { git.Oid.fromString('1810DFF58D8A660512D4832E740F692884338CCD'); });
  test.done();
};

// Oid::Sha
exports.sha = function(test) {
  test.expect(3);
  var sha = '1810DFF58D8A660512D4832E740F692884338CCD';
  var testOid = git.Oid.fromString(sha);

  // Test for function
  helper.testFunction(test.equals, testOid.sha, 'Oid::Sha');

  // Test valid hex id string
  test.equals(sha, testOid.sha().toUpperCase(), 'Valid hex id String');
  test.done();
};
