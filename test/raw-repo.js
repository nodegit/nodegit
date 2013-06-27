var git = require('../').raw,
    rimraf = require('rimraf'),
    path = require('path'),
    fs = require('fs');

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

// Repo
exports.open = function(test){
  test.expect(9);

  // Test for function
  helper.testFunction(test.equals, git.Repo, 'Repo');

  // Test for function
  helper.testFunction(test.equals, git.Repo.open, 'Repo::Open');

  // Test path argument existence
  helper.testException(test.ok, function() {
    git.Repo.open();
  }, 'Throw an exception if no path');

  // Test callback argument existence
  helper.testException(test.ok, function() {
    git.Repo.open('some/path');
  }, 'Throw an exception if no callback');

  // Test invalid repository
  git.Repo.open('/etc/hosts', function(error) {
    test.equals(error.message, "The `.git` file at '/private/etc/hosts' is malformed");

    // Test valid repository
    git.Repo.open(path.resolve('../.git'), function(error, repo) {
      test.ok(repo instanceof git.Repo, 'Invocation returns an instance of Repo');
      test.equals(null, error, 'Valid repository error code');
      test.done();
    });
  });
};

// Repo::Init
exports.init = function(test) {
  test.expect(7);

  // Test for function
  helper.testFunction(test.equals, git.Repo.init, 'Repo::Init');

  // Test path argument existence
  helper.testException(test.ok, function() {
    git.Repo.init();
  }, 'Throw an exception if no path');

  // Test is_bare argument existence
  helper.testException(test.ok, function() {
    git.Repo.init("some/path");
  }, 'Throw an exception if no is_bare');

  // Test callback argument existence
  helper.testException(test.ok, function() {
    git.Repo.init("some/path", true);
  }, 'Throw an exception if no callback');

  // Cleanup, remove test repo directory - if it exists
  rimraf('./test.git', function() {
    // Create bare repo and test for creation
    git.Repo.init('./test.git', true, function(error, path, is_bare) {
      test.equals(null, error, 'Successfully created bare repository');
      // Verify repo exists
      git.Repo.open('./test.git', function(error, path) {
        test.equals(null, error, 'Valid repository created');

          // Cleanup, remove test repo directory
          rimraf('./test.git', function() {
            test.done();
          });
      });
    });
  });
};
