/**/
var git = require( '../' );

var _Repo = function( path, callback ) {
  // Public namespace
  var self = {};

  // Private internal use variables
  var _commits = [],
      _error = git.error();

  function error( err ) {
    if(err !== 0) {
      return _error.error.strError( err );
    }

    return 0;
  }


  // Internal reference to a Git repository
  self.repo = new git.git2.Repo();

  // Find all commits under a given repository and optionally filtered further by a Ref
  //self.commits = function( obj ) {
  //  obj = obj || "HEAD";

  //  if( commits.length ) {
  //    return commits;
  //  }
  //  else {
  //    
  //  }
  //};

  // Find a single commit
  self.commit = function( sha, callback ) {
    var oid = new git.git2.Oid(),
        commit = new git.git2.Commit();

    if(!callback) { return; }

    oid.mkstr( sha );

    commit.lookup( self.repo, oid, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = error( args[0] );

      callback.apply( commit, args.concat( commit ) );
    });
  };

  self.find = function( name, callback ) {
    var ref = new git.git2.Ref();
    
    if(!callback) { return; }

    self.repo.lookupRef( ref, name, function() {
      var args = Array.prototype.slice.call( arguments ),
          ref = git.ref( ref );

      args[0] = error( args[0] );

      callback.apply( ref, args.concat( ref ) );
    });
  };

  self.init = function( path, is_bare, callback ) {
    if(!callback) { return; }

    self.repo.init( path, is_bare, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = error( args[0] );

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
    if(!callback) { return; }

    self.repo.open( path, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = error( args[0] );

      callback.apply( self, args.concat( self ) );
    });
  }
  else if( path ) {
    self.repo.open( path );
  }

  return self;
};

exports.repo = _Repo;
