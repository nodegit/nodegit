var git = require( '../' );

var _Commit = function( obj ) {
  var self = {};

  if( obj instanceof git.raw.Repo ) {
    self.repo = obj;
    self.commit = new git.raw.Commit( obj );
  }
  else if( obj instanceof git.raw.Commit ) {
    self.commit = obj;
  }

  Object.defineProperty( self, 'history', {
    get: function() {
      // Partially apply the commit
      var revwalk = git.revwalk( self.repo );
      revwalk.each = function( callback ) {
        return function( callback ) {
          return revwalk.walk.apply( self.commit, [ self.commit, callback ] );
        };
      }();

      return revwalk;
    },
    enumerable: true
  });

  Object.defineProperty( self, 'sha', {
    get: function() {
      var oid = new git.raw.Oid();
      self.commit.id( oid );

      return oid.toString( 40 );
    },
    enumerable: true
  });


  Object.defineProperty( self, 'msg', {
    get: function() {
      return self.commit.messageShort();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'message', {
    get: function() {
      return self.commit.message();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'time', {
    get: function() {
      return new Date( self.commit.time() * 1000 );
    },
    enumerable: true
  });

  Object.defineProperty( self, 'offset', {
    get: function() {
      return self.commit.timeOffset();
    },
    enumerable: true
  });

  Object.defineProperty( self, 'author', {
    get: function() {
      var sig = new git.raw.Sig();
      
      self.commit.author( sig );

      return git.sig( sig );
    },
    enumerable: true
  });

  self.lookup = function( oid, callback ) {
    self.commit.lookup( self.repo, oid, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = git.util().error( args[0] );

      callback.apply( self, args.concat( self ) );
    });
  };

  //self.tree = function() {
  //  var tree = new git.raw.Tree( self.repo );
  //  if( tree.error ) {
  //    throw git.error( tree.error );
  //  }
  //  else {
  //    self.commit.tree( tree );
  //  }

  //  return git.tree( tree );
  //};

  return self;
};

exports.commit = _Commit;
