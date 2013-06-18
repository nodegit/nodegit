var git = require('../');

// Helper functions
var helper = {
  // Test if obj is a true function
  testFunction: function(test, obj, label) {
    // The object reports itself as a function
    test(typeof obj, 'function', label + ' reports as a function.');
    // This ensures the repo is actually a derivative of the Function [[Class]]
    test(toString.call(obj), '[object Function]', label + ' [[Class]] is of type function.');
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

exports.method = function(test){
  test.expect(2);
  helper.testFunction(test.equals, git.commit, 'Oid');
  test.done();
};

var knownSha = 'fce88902e66c72b5b93e75bdb5ae717038b221f6';

exports.fromStringAndSha = function(test) {
  test.expect(1);
  var oid = git.oid.fromString(knownSha);
  test.equal(oid.sha(), knownSha, 'SHA should match known value');
  test.done();
};
