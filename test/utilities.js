var git = require('../'),
    utilities = require('../lib/utilities');

var object = {
  property: 'value'
};

var details = {
  one: 'one',
  two: 'two'
};

// Repo
exports.basic = function(test){
  test.expect(2);

  object = utilities.applyProperties(details, object);
  test.equal(object.one, details.one, 'Properties should match');
  test.equal(object.two, details.two, 'Properties should match');

  test.done();
};
