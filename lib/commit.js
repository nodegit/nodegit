var git = require( '../' );

var _Commit = function( obj ) {
  var self = {};

  if( obj instanceof git.git2.Repo ) {
    self.repo = obj;
    self.commit = new git.git2.Commit( obj );
  }
  else if( obj instanceof git.git2.Commit ) {
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

  self.author = function() {
    var sig = new git.git2.Sig();
    
    self.commit.author( sig );

    return git.sig( sig );
  };

  return self;
};

exports.commit = _Commit;
