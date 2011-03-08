var git = require( '../' );

var _Repo = function( path, callback ) {
  // Public namespace
  var self = {};

  // Private internal use variables
  var _commits = [];

  // Internal reference to a Git repository
  self.repo = new git.raw.Repo();


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

  //self.find = function( name, callback ) {
  //  var ref = new git.raw.Ref( repo );
  //  
  //  if( !callback ) { return; }

  //  self.repo.lookupRef( ref, name, function() {
  //    var args = Array.prototype.slice.call( arguments ),
  //        ref = git.ref( ref );

  //    args[0] = git.util().error( args[0] );

  //    callback.apply( ref, args.concat( ref ) );
  //  });
  //};

  self.init = function( path, is_bare, callback ) {
    if( !callback ) { return; }

    self.repo.init( path, is_bare, function() {
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

  // Constructor use
  if( path && callback ) {
    if( !callback ) { return; }

    self.repo.open( path, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = git.util().error( args[0] );

      callback.apply( self, args.concat( self ) );
    });
  }
  else if( path ) {
    self.repo.open( path );
  }

  return self;
};

exports.repo = _Repo;
