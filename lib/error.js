var git = require( '../' );

var _Error = function( error ) {
  var self = {};

  // Internal reference to a Git reference
  self.error = error || new git.git2.Error();

  self.toString = function( err ) {
    return self.error.strError( err );
  };

  return self;
};

exports.error = _Error;
