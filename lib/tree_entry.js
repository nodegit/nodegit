var git = require( '../' );

var _TreeEntry = function( obj ) {
  var self = {};

  Object.defineProperty( self, 'name', {
    get: function() {
      return self.entry.name();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'content', {
    get: function() {
      var obj = git.obj(),
          blob;

      self.entry.toObject( obj.obj );

      blob = git.blob( obj.owner() );
      blob.lookup( oid.oid );

      return blob.content();
    },
    enumerable: true
  });

  // Internal references to Git references
  if ( obj instanceof git.raw.TreeEntry ) {
    self.entry = obj;
  }
  else {
    self.entry = new git.raw.TreeEntry();
  }

  return self;
};

exports.entry = _TreeEntry;
