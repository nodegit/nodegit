var git = require( '../' );

var _RevWalk = function( repo ) {
  var self = {};

  // Internal reference to a Git reference
  self.revwalk = revwalk || new git.git2.RevWalk( repo );

  // Walk will map to the next method
  self.walk = function( commit, callback ) {
    if( !callback ) { return; }

    self.revwalk.push( commit );

    revwalk.next( commit, function() {
      var args = Array.prototype.slice.call( arguments );

      args[0] = error( args[0] );


      callback.apply( commit, args.concat( commit ) );
    });
  };

  return self;
};

exports.revwalk = _RevWalk;
