var git = require( '../' );

var _Repo = function( path, callback ) {
  // Public namespace
  var self = {};

  // Private internal use variables
  var _commits = [];

  // Internal reference to a Git repository
  self.repo = new git.git2.Repo();

  // Work with a specific branch
  //self.branch = function( name, callback ) {
  //  var branch = new git.git2.Ref( self.repo );
  //  
  //  self.repo.lookupRef( branch, 'refs/heads/'+ name, function() {
  //    var args = Array.prototype.slice.call( arguments );
  //    args[0] = git.error().toString( args[0] );

  //    callback.apply( branch, args.concat( branch ) );
  //  });
  //};

  // Find a single commit
  self.commit = function( sha, callback ) {
    var commit = git.commit( self.repo );
    return commit.lookup( sha, callback );
  };

  //self.find = function( name, callback ) {
  //  var ref = new git.git2.Ref( repo );
  //  
  //  if( !callback ) { return; }

  //  self.repo.lookupRef( ref, name, function() {
  //    var args = Array.prototype.slice.call( arguments ),
  //        ref = git.ref( ref );

  //    args[0] = git.error().toString( args[0] );

  //    callback.apply( ref, args.concat( ref ) );
  //  });
  //};

  self.init = function( path, is_bare, callback ) {
    if( !callback ) { return; }

    self.repo.init( path, is_bare, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = args[0] ? git.error().toString( args[0] ) : args[0];

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

      args[0] = args[0] ? git.error().toString( args[0] ) : args[0];

      callback.apply( self, args.concat( self ) );
    });
  }
  else if( path ) {
    self.repo.open( path );
  }

  return self;
};

exports.repo = _Repo;
