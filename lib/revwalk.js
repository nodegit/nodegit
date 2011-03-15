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
  self.walk = function( oid, callback ) {
    if( !callback ) { return; }

    self.revwalk.push( oid );

    var cont = true, i = 0;

    function walk() {
      if( !cont ) { return; }
      var _tmp = git.oid();

      self.revwalk.next( _tmp.oid, function( err ) {
        if( err ) { return; }

        git.commit( self.repo ).lookup( _tmp.oid, function( err ) {
          if( err ) { return; }

          if( callback.apply( this, [ i, this ] ) === false ) {
            cont = false;
          }

          i = i + 1;
          walk();
        });
      });
    }

    walk();
  };

  return self;
};

exports.revwalk = _RevWalk;
