var git = require( '../' );

git.repo( './dummyrepo/.git', function( err, repo ) {
  if( err ) { throw err; }
 
  // Read a commit when you know the sha1
  repo.commit( '2f6cbe055f1a6ca0a3ba524ba88a7806ba507a89', function( err, commit ) {
    console.log( commit.author().name );
  });

  // Read a commit when you know the name
  repo.head( 'master', function( err, head ) {
    git.commit( repo.repo ).lookup( head.oid().oid, function( err, commit ) {
      console.log( commit.author().name );
    });
  });
});
