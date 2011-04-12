var git = require( '../' ).raw
  , path = require( 'path' );

var repo = new git.Repo();
repo.open( path.resolve( '../.git' ), function() {
  var oid = new git.Oid();
  oid.mkstr( '59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5' );

  var commit = new git.Commit();
  commit.lookup( repo, oid, function( err ) {
    var tree = new git.Tree( repo ),
        entry = new git.TreeEntry(),
        blob = new git.Blob( repo );

    if( !commit.tree( tree ) && tree.entryCount() > 1 ) {
      tree.entryByIndex( entry, 1 );
      entry.toObject( repo, blob );

      console.log( entry.name() + ':' );
      console.log( blob.rawSize() );
      console.log( blob.rawContent() );
    }
  });
});
