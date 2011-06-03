var git = require( '../' );

git.repo( '../.git', function( err, repo ) {
  if( err ) { throw err; }

  repo.branch( 'master', function( err, branch ) {
    if( err ) { throw err; }

    branch.tree().walk().on('entry', function( idx, entry ) {
      //console.log(entry.entry);
      console.log( entry.name, entry.attributes );
      //console.log( entry.content );
    });

    //branch.tree().entry('example/raw-blob.js', function( entry ) {
    //  if( entry ) {
    //    console.log(entry.name);
    //  }
    //  else {
    //    console.log('not found');
    //  }
    //});
  });
});
