var git2 = require('../build/default/git2');

var Repo = function( path, callback ) {
  var self = {};

  // Internal reference to a Git repository
  self.repo = new git2.Repo();

  self.commit = function( sha, callback ) {
    var oid = new git2.Oid(),
        commit = new git2.Commit();

    oid.mkstr( sha );

    commit.lookup( self.repo, oid, function() {
      var args = Array.prototype.slice.call( arguments );
      callback && callback.apply( commit, (args.push( commit ), args) );
    });
  };

  self.init = function( path, is_bare, callback ) {
    self.repo.init( path, is_bare, function() {
      var args = Array.prototype.slice.call( arguments );
      callback && callback.apply( self, (args.push( self ), args) );
    });

    return self;
  };

  self.free = function() { };

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
