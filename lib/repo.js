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

    self.ref( 'refs/heads/' + name, function( err, ref ) {
      if( err ) { throw err; }

      git.commit( self.repo ).lookup( ref.oid().oid, function() {
        var args = Array.prototype.slice.call( arguments );
        args[0] = git.util().error( args[0] );

        callback.apply( this, args.concat( this ) );
      });
    });
  };

  // Work with a specific head reference
  self.ref = function( name, callback ) {
    if( !callback ) { return; }

    var ref = git.ref( self.repo );
    
    self.repo.lookupRef( ref.ref, name, function() {
      var args = Array.prototype.slice.call( arguments );
      args[0] = git.util().error( args[0] );

      callback.apply( ref, args.concat( ref ) );
    });
  };

  // Find a single commit
  self.commit = function( sha, callback ) {
    if( !callback ) { return; }

    var oid = git.oid( sha );

    git.commit( self.repo ).lookup( oid.oid, callback );
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
