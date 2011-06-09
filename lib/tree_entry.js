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

  self.dir = '';

  Object.defineProperty( self, 'name', {
    get: function() {
      return self.dir + self.entry.name();
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

  Object.defineProperty( self, 'id', {
    get: function() {
      var oid = git.oid();
      self.entry.id( oid.oid );

      return oid;
    },
    enumerable: true
  });

  Object.defineProperty( self, 'sha', {
    get: function() {
      var oid = self.id;
      //console.log(self.id);

      return oid.oid.toString( 40 );
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
    return self.attributes !== 16384;
  };

  self.isDir = function() {
    return self.attributes === 16384;
  };

  self.tree = function() {
    var tree = new git.raw.Tree( self.repo );
    if( tree.error ) {
      return git.error( tree.error );
    }
    else {
      if( tree.lookup(self.repo, self.id.oid) ) {
        return git.error( tree.error );
      }
    }

    return git.tree( tree );
  };

  return self;
};

exports.entry = _TreeEntry;
