var git2 = require('../build/default/git2');

var Error = function( error ) {
  var self = {};

  // Internal reference to a Git reference
  self.error = error || new git2.Error();

  return self;
};

exports.error = Error;
