var git = require( '../' );

var _TreeEntry = function( obj ) {
  var self = {};

  if( obj instanceof git.raw.TreeEntry ) {
    self.entry = obj;
  }
  else {
    self.entry = new git.raw.TreeEntry();
    self.repo = obj;
  }

  Object.defineProperty( self, 'name', {
    get: function() {
      return self.entry.name();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'content', {
    get: function() {
      var blob = git.blob( self.repo );

      self.entry.toObject( blob.blob );

      return blob.raw;
    },
    enumerable: true
  });

  return self;
};

exports.entry = _TreeEntry;
