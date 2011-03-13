var git = require( 'nodegit' );

git.repo( '../.git', function( err, repo ) {
  if( err ) { throw err; }

  repo.branch( 'master', function( err, branch ) {
    if( err ) { throw err; }

    branch.tree().each( function( i, entry ) {
      console.log( entry.name );

      console.log( entry.contents );

      var blob = git.blob( repo.repo );
      entry.entry.toObject( blob.blob );
      console.log( blob.blob.rawContent() );
    });
  });
});
