var git = require('../').raw,
    path = require('path'),
    rimraf = require('rimraf');

var testRepo = new git.Repo();

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
 * Blob constructor
 */
exports.constructor = function(test){
  test.expect(3);
  helper.testFunction(test.equals, git.Blob, 'Blob');
  test.ok(new git.Blob() instanceof git.Blob, 'Invocation returns an instance of Blob');
  test.done();
};

// Blob::Lookup
exports.lookup = function(test) {
  var testOid = new git.Oid(),
      testRef = new git.Reference(testRepo),
      testBlob = new git.Blob();

  test.expect(5);

  // Test for function
  helper.testFunction(test.equals, testBlob.lookup, 'Blob::Lookup');

  // Test repo argument existence
  helper.testException(test.ok, function() {
    testBlob.lookup();
  }, 'Throw an exception if no repo Object');

  // Test Oid argument existence
  helper.testException(test.ok, function() {
    testBlob.lookup(testRepo);
  }, 'Throw an exception if no oid Object');

  // Test Callback argument existence
  helper.testException(test.ok, function() {
    testBlob.lookup(testRepo, testOid);
  }, 'Throw an exception if no callback Object');

  testRepo.open(path.resolve('../.git'), function() {
    // @todo actually lookup
    test.done();
  });
};

// Blob::RawContent
exports.rawContent = function(test) {
  var testOid = new git.Oid(),
      testBlob = new git.Blob(),
      testCommit = new git.Commit();

  test.expect(2);

  // Test for function
  helper.testFunction(test.equals, testBlob.rawContent, 'Blob::RawContent');

  test.done();
};

// Blob::Free
exports.free = function(test) {
  var testOid = new git.Oid(),
      testBlob = new git.Blob();

  test.expect(2);

  // Test for function
  helper.testFunction(test.equals, testBlob.free, 'Blob::Free');

  test.done();
};

// Blob::CreateFromFile
exports.createFromFile = function(test) {
  var testOid = new git.Oid(),
      testBlob = new git.Blob();

  test.expect(2);

  // Test for function
  helper.testFunction(test.equals, testBlob.createFromFile, 'Blob::CreateFromFile');

  test.done();
};

// Blob::CreateFromBuffer
exports.createFromBuffer = function(test) {
  var testOid = new git.Oid(),
      testBlob = new git.Blob();

  test.expect(2);

  // Test for function
  helper.testFunction(test.equals, testBlob.createFromBuffer, 'Blob::CreateFromBuffer');

  test.done();
};
