var git = require( '../' );

var _Object = function( obj ) {
  var self = {};

  if( obj instanceof git.raw.Object ) {
    self.obj = obj;
  }
  else {
    self.obj = new git.raw.Object();
  }

  Object.defineProperty( self, 'id', {
    get: function() {
    },
    enumerable: true
  });

  Object.defineProperty( self, 'type', {
    get: function() {
      return self.obj.type();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'length', {
    get: function() {
      return self.obj.size();
    },
    enumerable: true
  });
  
  Object.defineProperty( self, 'isLoose', {
    get: function() {
      return self.obj.typeIsLoose();
    },
    enumerable: true
  });

  self.id = function() {
    var oid = git.oid();

    self.obj.id( oid.oid ); 

    return oid;
  };

  self.owner = function() {
    var repo = git.repo();

    self.obj.owner( repo.repo );

    return repo;
  };

  self.toString = function() {
    return self.obj.type2String();
  };

  self.toType = function( type ) {
    return self.obj.toType( type );
  };

  self.free = function() {
    return self.obj.free();
  };

  return self;
};

exports.obj = _Object;
