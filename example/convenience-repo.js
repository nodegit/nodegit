var git = require( 'nodegit2' );

git.repo( '.git', function( err, path ) {
  if(err === 0) {
    this.commit( '5f2aa9407f7b3aeb531c621c3358953841ccfc98', function( err, details, commit ) {
      console.log( err );
      //console.log( 'Message: '+ details.message.trim() );
      console.log( details );
      // console.log( 'Short message: '+ details.short_message ); -- No short message set
    });
  }
});
