var git = require( '../' );

// Read the current repository
git.repo( './dummyrepo/.git', function( err, path, repo ) {
  if( err ) { throw err; }
 
  // Read a commit
  repo.commit( '2f6cbe055f1a6ca0a3ba524ba88a7806ba507a89', function( err, commit ) {
    if( err ) { throw err; }

    console.log( commit.msg() );

    var author = commit.author();
    console.log( author.name );
    console.log( author.email );
  });

  repo.head( 'master', function( err, path, head ) {
    git.commit( head ).author().name;
  });
});
