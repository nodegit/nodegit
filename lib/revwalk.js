var git = require( '../' );

var _RevWalk = function( obj ) {
  var self = {};

  if( obj instanceof git.raw.Repo ) {
    self.repo = obj;
    self.revwalk = new git.raw.RevWalk( obj );
  }
  else if( obj instanceof git.raw.RevWalk ) {
    self.revwalk = obj;
  }

  // Walk will map to the next method
  self.walk = function( commit, callback ) {
    if( !callback ) { return; }

    self.revwalk.push( commit );

    function walk() {
      var _tmp = git.commit( self.repo );
      self.revwalk.next( _tmp.commit, function( err ) {
        if( err ) { return; }

        var args = Array.prototype.slice.call( arguments );
        args[0] = git.util().error( args[0] );

        callback.apply( _tmp, args.concat( _tmp ) );
        walk();
      });
    }

    walk();
  };

  return self;
};

exports.revwalk = _RevWalk;
