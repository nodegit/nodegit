var git2 = require('../build/default/git2');

var Repo = function( path, callback ) {
  var self = {};

  // Internal reference to a Git repository
  self.repo = new git2.Repo();

  self.commit = function( sha, callback ) {
    var oid = new git2.Oid(),
        commit = new git2.Commit();

    console.log( oid.mkstr( sha ) );

    callback && callback.appy( self, arguments );
  };

  self.init = function( path, is_bare, callback ) {
    self.repo.init( path, is_bare, function() {
      callback && callback.apply( self, arguments );
    });
  };

  self.free = function() { };

  // Constructor use
  if( path && callback ) {
    self.repo.open( path, function() {
      callback && callback.apply( self, arguments );
    });
  }
  else if( path ) {
    self.repo.open( path );
  }

  return self;
};

exports.repo = Repo;
