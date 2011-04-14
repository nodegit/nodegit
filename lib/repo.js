var git = require( '../' ),
    path = require( 'path' );

var _Repo = function( dir, callback ) {
  // Public namespace
  var self = {};

  self.repo = new git.raw.Repo();

  if( dir && callback ) {
    if( !callback ) { return; }

    self.repo.open( path.resolve && path.resolve( dir ) || dir, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = git.util().error( args[0] );

      callback.apply( self, args.concat( self ) );
    });
  }
  else if( dir ) {
    self.repo.open( path.resolve && path.resolve( dir ) || dir );
  }

  // Look up a branch and find its tree
  self.branch = function( name, callback ) {
    if( !callback ) { return; }

    git.ref( self.repo ).lookup( 'refs/heads/' + name, function( err, ref ) {
      if( err ) {
        var args = Array.prototype.slice.call( arguments );
        args[0] = git.util().error( args[0] );

        callback.apply( this, args.concat( this ) );
      }

      git.commit( self.repo ).lookup( self.repo, ref.oid().oid, function() {
        var args = Array.prototype.slice.call( arguments );
        args[0] = git.util().error( args[0] );

        callback.apply( this, args.concat( this ) );
      });
    });
  };

  // Find a single commit
  self.commit = function( sha, callback ) {
    if( !callback ) { return; }

    var oid = git.oid( sha );

    git.commit().lookup( self.repo, oid.oid, callback );
  };

  self.init = function( dir, is_bare, callback ) {
    if( !callback ) { return; }

    self.repo.init( path.resolve && path.resolve( dir ) || dir, is_bare, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = git.util().error( args[0] );

      callback.apply( self, args.concat( self ) );
    });

    return self;
  };

  self.free = function() { 
    self.repo.free();
    delete self.repo;
  };

  return self;
};

exports.repo = _Repo;
