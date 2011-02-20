var git = require( 'nodegit2' );

git.repo( '.git', function( err, path ) {
  if(err === 0) {
    console.log( 'Successfully loaded repository.' );
    this.commit( '5f2aa9407f7b3aeb531c621c3358953841ccfc98', function( err ) {
      console.log( err );
    });
  }
});
