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

// Ref
exports.constructor = function(test){
  test.expect(2);

  // Test for function
  helper.testFunction(test.equals, git.Reference, 'Reference');

  test.done();
};

// Ref::Lookup
exports.lookup = function(test) {

  test.expect(5);

  // Test for function
  helper.testFunction(test.equals, git.Reference.lookup, 'Ref::Lookup');

  // Test repo argument existence
  helper.testException(test.ok, function() {
    git.Reference.lookup();
  }, 'Throw an exception if no repo');

  git.Repo.open('../.git', function(error, repo) {
    // Test name argument existence
    helper.testException(test.ok, function() {
      git.Reference.lookup(repo);
    }, 'Throw an exception if no name');

    // Test callback argument existence
    helper.testException(test.ok, function() {
      git.Reference.lookup(repo, 'refs/heads/master');
    }, 'Throw an exception if no callback');

    // Cleanup, remove test repo directory - if it exists
    rimraf('./test.git', function() {
    //  // Create bare repo and test for creation
    //  repo.init('./test.git', true, function(err, path, is_bare) {
    //    test.equals(0, err, 'Successfully created bare repository');
    //    // Verify repo exists
    //    repo.open('./test.git', function(err, path) {
    //      test.equals(0, err, 'Valid repository created');
    //      test.equals(true, is_bare, 'Returns valid is_bare value');

    //        repo.free();

    //        // Cleanup, remove test repo directory
    //        rimraf('./test.git', function() {
              test.done();
    //        });
    //    });
    //  });
    });
  });
};
