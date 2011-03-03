var git = require( '../' );

// Read the current repository
git.repo( '.git', function( err, path, repo ) {
  if( err ) { throw err; }
 
  // Read a commit
  this.commit( '75054b7130858db1c1cba13cf8a8febb26b14771', function( err, commit ) {
    if( err ) { throw err; }

    console.log( this.msg() );

    var author = commit.author();
    console.log( author.name );
    console.log( author.email );
  });
});
