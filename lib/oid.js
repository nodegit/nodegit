var git = require( '../' );

var _Oid = function( obj ) {
  var self = {};

  if( obj instanceof git.raw.Oid ) {
    self.oid = obj;
  }
  else {
    self.oid = new git.raw.Oid();

    if( typeof obj === 'string' ) {
      self.oid.mkstr( obj );
    }
  }

  return self;
};

exports.oid = _Oid;
