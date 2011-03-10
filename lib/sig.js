var git = require( '../' );

var _Sig = function( obj ) {
  var self = {};

  Object.defineProperty( self, 'name', {
    get: function() {
      return self.sig.name();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'email', {
    get: function() {
      return self.sig.email();
    },
    enumerable: true
  });

  // Internal references to Git references
  if( obj instanceof git.raw.Repo ) {
    // TODO: Add support for creation
  }
  else if ( obj instanceof git.raw.Sig ) {
    self.sig = obj;
  }

  return self;
};

exports.sig = _Sig;
