/*global require: true, jQuery: false */
var git = require( 'nodegit2' );

var Repo = function( path, callback ) {
  // Public namespace
  var self = {};

  // Private internal use variables
  var _commits = [];

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

    oid.mkstr( sha );

    commit.lookup( self.repo, oid, function() {
      var args = Array.prototype.slice.call( arguments );
      callback && callback.apply( commit, (args.push( commit ), args) );
    });
  };

  self.find = function( name ) {
    var ref = new git.git2.Ref();
    self.repo.lookup_ref( ref, name, function( ref ) {
      var args = Array.prototype.slice.call( arguments );
      callback && callback.apply( ref, (args.push( git.ref(ref) ), args) );
    });
  };

  self.init = function( path, is_bare, callback ) {
    self.repo.init( path, is_bare, function() {
      var args = Array.prototype.slice.call( arguments );
      callback && callback.apply( self, (args.push( self ), args) );
    });

    return self;
  };

  self.free = function() { 
    self.repo.free();
    delete self.repo;
  };

  // Constructor use
  if( path && callback ) {
    self.repo.open( path, function() {
      var args = Array.prototype.slice.call( arguments );
      callback && callback.apply( self, (args.push( self ), args) );
    });
  }
  else if( path ) {
    self.repo.open( path );
  }

  return self;
};

exports.repo = Repo;
