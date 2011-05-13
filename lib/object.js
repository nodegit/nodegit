var git = require( '../' );

var _Object = function( obj ) {
  var self = {};

  if( object instanceof git.raw.Object ) {
    self.object = obj;
  }
  else {
    self.object = new git.raw.Object();
  }

  Object.defineProperty( self, 'id', {
    get: function() {
    },
    enumerable: true
  });

  Object.defineProperty( self, 'type', {
    get: function() {
      return self.object.type();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'length', {
    get: function() {
      return self.object.size();
    },
    enumerable: true
  });
  
  Object.defineProperty( self, 'isLoose', {
    get: function() {
      return self.object.typeIsLoose();
    },
    enumerable: true
  });

  self.id = function() {
    var oid = git.oid();

    self.object.id( oid.oid ); 

    return oid;
  };

  self.owner = function() {
    var repo = git.repo();

    self.object.owner( repo.repo );

    return repo;
  };

  self.toString = function() {
    return self.object.type2String();
  };

  self.toType = function( type ) {
    return self.object.toType( type );
  };

  self.free = function() {
    return self.object.free();
  };

  return self;
};

exports.object = _Object;
