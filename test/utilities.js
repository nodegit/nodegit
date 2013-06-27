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
exports.successError = function(test) {
  test.expect(2);
  var e = new Error("Message");
  utilities.success(e, function(error) {
    test.notEqual(error, null, 'Error should not be null');
    test.equal(error, e, 'Error message should match input');
    test.done();
  });
};
