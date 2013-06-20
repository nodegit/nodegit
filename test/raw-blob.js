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
  test.throws(function() { new git.Blob(); });
  test.done();
};

// Blob::Lookup
exports.lookup = function(test) {
  var testOid = git.Oid.fromString('fce88902e66c72b5b93e75bdb5ae717038b221f6'),
      testRef = new git.Reference(testRepo);

  test.expect(5);

  // Test for function
  helper.testFunction(test.equals, git.Blob.lookup, 'Blob::Lookup');

  // Test repo argument existence
  helper.testException(test.ok, function() {
    git.Blob.lookup();
  }, 'Throw an exception if no repo Object');

  // Test Oid argument existence
  helper.testException(test.ok, function() {
    git.Blob.lookup(testRepo);
  }, 'Throw an exception if no oid Object');

  // Test Callback argument existence
  helper.testException(test.ok, function() {
    git.Blob.lookup(testRepo, testOid);
  }, 'Throw an exception if no callback Object');

  testRepo.open(path.resolve('../.git'), function() {
    // @todo actually lookup
    test.done();
  });
};

// Blob::RawContent
exports.rawContent = function(test) {
  // This shouldn't fail unless someone rewrites history:
  var testOid = git.Oid.fromString('111dd657329797f6165f52f5085f61ac976dcf04'),
      testCommit = new git.Commit();
  test.expect(3);
  testRepo.open(path.resolve('../.git'), function(err, repo) {
    git.Blob.lookup(repo, testOid, function(err, blob) {
      // Test for function
      helper.testFunction(test.equals, blob.content, 'Blob::RawContent');
      test.equals(blob.content().toBuffer(7).toString(), "@import");

      test.done();
    });
  });
};

// Blob::CreateFromFile
exports.createFromFile = function(test) {
  var testOid = git.Oid.fromString('fce88902e66c72b5b93e75bdb5ae717038b221f6');

  test.expect(2);

  // Test for function
  helper.testFunction(test.equals, git.Blob.createFromFile, 'Blob::CreateFromFile');

  test.done();
};

// Blob::CreateFromBuffer
exports.createFromBuffer = function(test) {
  var testOid = git.Oid.fromString('fce88902e66c72b5b93e75bdb5ae717038b221f6');
  test.expect(2);

  // Test for function
  helper.testFunction(test.equals, git.Blob.createFromBuffer, 'Blob::CreateFromBuffer');

  test.done();
};
