var git = require( '../' );

var _Error = function( error ) {
  var self = {};

  // Internal reference to a Git reference
  self.error = error || new git.git2.Error();

  return self;
};

exports.error = _Error;
