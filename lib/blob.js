var git = require( '../' );

var _Blob = function( obj ) {
  var self = {};

  if( obj instanceof git.raw.Repo ) {
    self.repo = obj;
    self.blob = new git.raw.Blob( obj );
  }
  else if( obj instanceof git.raw.Blob ) {
    self.blob = obj;
  }

  Object.defineProperty( self, 'raw', {
    get: function() {
      return self.blob.rawContent().toString();
    },
    enumerable: true
  });

  self.lookup = function( oid ) {
    self.blob.lookup( self.repo, oid, function() {
      var args = Array.prototype.slice.call( arguments );
      args[0] = git.util().error( args[0] );

      callback.apply( self, args.concat( self ) );
    });
  };

  return self;
};

exports.blob = _Blob;
