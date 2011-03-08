var git = require( '../' );

var _Error = function( obj ) {
  var self = {};

  if( obj instanceof git.raw.Error ) {
    self.error = obj;
  }
  else {
    if( !self.error ) {
      self.error = new git.raw.Error();
    }

    if( typeof obj === 'number' ) {
      return self.error.strError( obj );
    }
  }

  return self;
};

exports.error = _Error;
