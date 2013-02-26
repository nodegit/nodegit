var git = require( '../' );

git.repo( '../.git', function( err, repo ) {
  if( err ) { throw new Error( err ); }

  repo.commit( '59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5', function( err, commit ) {
    if( err ) { throw new Error( err ); }

    var history = commit.history();
    history.on( 'commit', function() {
      // console.log(arguments);
    });

    history.on( 'end', function( commits ) {
      // Read first commit tree
      var tree = commits[0].tree();

      // Synchronous
      tree.walk().on('entry', function( i, entry ) {
        console.log( entry.content );
        return false;
      });

      // Asynchronous - not yet implemented
      //tree.walk().on( 'entry', function( err, i, entry ) {
      //  console.log( entry );
      //});
    });
  });

  //repo.branch( 'master', function( err, branch ) {
  //  if( err ) { throw new Error( err ); }

  //  var history = branch.history();
  //  console.log( history );

  //  //branch.tree().each( function( i, entry ) {
  //  //  console.log( entry.name );
  //  //  console.log( entry.contents );

  //  //});
  //});
});
