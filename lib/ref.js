var git = require( '../' );

var _Ref = function( obj ) {
  var self = {};

  if( obj instanceof git.git2.Repo ) {
    self.repo = obj;
    self.ref = new git.git2.Ref( obj );
  }
  else if( obj instanceof git.git2.Ref ) {
    self.ref = obj;
  }

  self.oid = function() {
    var oid = git.oid();

    self.ref.oid( oid.oid );

    return oid;
  };

  return self;
};

exports.ref = _Ref;
