var git = require('../').raw,
    path = require('path'),
    rimraf = require('rimraf');

var testRepo = new git.Repo();

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

// Blob
exports.constructor = function(test){
  test.expect(3);

  // Test for function
  helper.testFunction(test.equals, git.Blob, 'Blob');

  // Ensure we get an instance of Blob
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

    //testOid.mkstr('59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5');

    //testCommit.lookup(testRepo, testOid, function(err) {
    //  var tree = new git.Tree(testRepo)
    //    , entry = new git.TreeEntry()
    //    , blob = new git.Blob(testRepo);

    //  if(!testCommit.tree(tree) && tree.entryCount() > 1) {
    //    tree.entryByIndex(entry, 1);
    //    entry.toObject(testRepo, blob);

    //    //console.log(entry.name() + ':');
    //    //console.log(blob.rawSize());
    //    //console.dir(blob.rawContent());
    //  }
    //});
    test.done();

  });
};

// Blob::RawContent
exports.rawContent = function(test) {
  var testOid = new git.Oid()
    , testBlob = new git.Blob()
    , testCommit = new git.Commit();

  test.expect(2);

  // Test for function
  helper.testFunction(test.equals, testBlob.rawContent, 'Blob::RawContent');

  testRepo.open(path.resolve('../.git'), function() {
    testOid.mkstr('59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5');

    testCommit.lookup(testRepo, testOid, function(err) {
      var tree = new git.Tree(testRepo),
          entry = new git.TreeEntry(),
          blob = new git.Blob(testRepo);

      //if(!testCommit.tree(tree) && tree.entryCount() > 1) {
      //  tree.entryByIndex(entry, 1);
      //  entry.toObject(testRepo, blob);

      //  //console.log(entry.name() + ':');
      //  //console.log(blob.rawSize());
      //  //console.dir(blob.rawContent());
      //}
    });
  });

  test.done();
};

// Blob::Close
exports.close = function(test) {
  var testOid = new git.Oid(),
      testBlob = new git.Blob();

  test.expect(2);

  // Test for function
  helper.testFunction(test.equals, testBlob.close, 'Blob::Close');

  test.done();
};

// Blob::CreateFromFile
exports.createFromFile = function(test) {
  var testOid = new git.Oid(),
      testBlob = new git.Blob();

  test.expect(2);

  // Test for function
  helper.testFunction(test.equals, testBlob.createFromFile, 'Blob::Close');

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
