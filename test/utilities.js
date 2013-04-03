var git = require('../'),
    utilities = require('../lib/utilities');

exports.successNoError = function(test){
  test.expect(0);

  if (utilities.success(null, function() { })) {
    test.done();
  }
};

/**
 * Test whether success function calls callback with error
 */
exports.successError = function(test){
  test.expect(3);
  utilities.success(new git.error('Message', git.raw.Error.codes.GITERR_INVALID), function(error) {
    test.notEqual(error, null, 'Error should not be null');
    test.equal(error.code, git.raw.Error.codes.GITERR_INVALID, 'Error code should match input');
    test.equal(error.message, 'Message', 'Error message should match input');
    test.done();
  });
};
