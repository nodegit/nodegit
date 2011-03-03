var git = require( '../' );

var _Error = function( obj ) {
  var self = {};

  if( obj instanceof git.git2.Error ) {
    self.error = obj;
  }
  else {
    if( !self.error ) {
      self.error = new git.git2.Error();
    }

    if( typeof obj === 'number' ) {
      return self.error.strError( obj );
    }
  }

  return self;
};

exports.error = _Error;
