var git = require( '../' );

var _Ref = function( obj ) {
  var self = {};

  if( obj instanceof git.raw.Repo ) {
    self.repo = obj;
    self.ref = new git.raw.Ref( obj );
  }
  else if( obj instanceof git.raw.Ref ) {
    self.ref = obj;
  }

  self.lookup = function( name, callback ) {
    if( !callback ) { return; }

    self.ref.lookup( self.repo, name, function() {
      var args = Array.prototype.slice.call( arguments );
      args[0] = git.util().error( args[0] );

      callback.apply( self, args.concat( self ) );
    });
  };

  self.oid = function() {
    var oid = git.oid();

    self.ref.oid( oid.oid );

    return oid;
  };

  return self;
};

exports.ref = _Ref;
