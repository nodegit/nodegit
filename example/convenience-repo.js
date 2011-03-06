var git = require( '../' );

git.repo( '/home/tim/Projects/nodegit2/.git', function( err, repo ) {
  if( err ) { throw err; }
 
  // Read a commit when you know the sha1
  //repo.commit( 'd29b7fecf71d0ef4887071ac18dc87f40c2fd4e1', function( err, commit ) {
  //  console.log( commit.tree() );
  //});

  // Read a commit when you know the name
  repo.head( 'master', function( err, head ) {
    git.commit( repo.repo ).lookup( head.oid().oid, function( err, commit ) {
      console.log( commit.tree().length );
    });
  });
});
