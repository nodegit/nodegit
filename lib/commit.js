var git = require( '../' );

var _Commit = function( obj ) {
  var self = {};

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

  if( obj instanceof git.raw.Repo ) {
    self.repo = obj;
    self.commit = new git.raw.Commit( obj );
  }
  else if( obj instanceof git.raw.Commit ) {
    self.commit = obj;
  }

  self.lookup = function( oid, callback ) {
    self.commit.lookup( self.repo, oid, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = git.util().error( args[0] );

      callback.apply( self, args.concat( self ) );
    });
  };

  self.msg = function() {
    return self.commit.messageShort();
  };

  self.message = function() {
    return self.commit.message();
  };

  self.time = function() {
    return new Date( self.commit.time() * 1000 );
  };

  self.author = function() {
    var sig = new git.raw.Sig();
    
    self.commit.author( sig );

    return git.sig( sig );
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
