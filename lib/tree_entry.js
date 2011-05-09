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

  Object.defineProperty( self, 'attributes', {
    get: function() {
      return self.entry.attributes();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'object', {
    get: function() {
      return self.entry.toObject();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'content', {
    get: function() {
      if( self.isFile() ) {
        var blob = git.blob( self.repo );

        self.entry.toObject( self.repo, blob.blob );

        return blob.raw;
      }

      return null;
    },
    enumerable: true
  });

  self.isFile = function() {
    return self.attributes === 33188;
  };

  self.isDir = function() {
    return self.attributes === 16384;
  };

  return self;
};

exports.entry = _TreeEntry;
