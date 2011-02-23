var git = require( 'nodegit2' );

var Error = function( error ) {
  var self = {};

  // Internal reference to a Git reference
  self.error = error || new git.git2.Error();

  return self;
};

exports.error = Error;
