var git = require( '../lib' ).raw
  , path = require( 'path' );

var repo = new git.Repo();
repo.open( path.resolve( '../.git' ), function() {
  var oid = new git.Oid();
  oid.mkstr( '3b7670f327dc1ca66e040f0c09cc4c3f1428eb49' );

  var commit = new git.Commit();
  commit.lookup( repo, oid, function( err ) {
    console.log( new git.Error().strError( err ) );
  });
});
