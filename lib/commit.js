var git = require( '../' );

var _Commit = function( obj ) {
  var self = {};

  // Internal references to Git references
  if( obj instanceof git.git2.Repo ) {
    self.commit = new git.git2.Commit( obj );
  }
  else if ( obj instanceof git.git2.Commit ) {
    self.commit = obj;
  }

  // TODO: Figure out how to get repo out of a commit
  self.lookup = function( sha, callback ) {
    var oid = new git.git2.Oid();

    if( !callback ) { return; }

    oid.mkstr( sha );

    self.commit.lookup( self.repo, oid, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = git.error().toString( args[0] );

      callback.apply( self, args.concat( self ) );
    });
  };

  self.short = function() {
    return self.commit.messageShort();
  };

  return self;
};

exports.commit = _Commit;
