var git = require('nodegit2');

// Read the current repository
git.repo( '.git', function( err, path, repo ) {
  // 
  this.find( 'HEAD', function( err, name, ref ) {
  //  console.log( ref );
  });

  // Read a commit
  this.commit( '5f2aa9407f7b3aeb531c621c3358953841ccfc98', function( err, details, commit ) {
    console.log( 'Message', details.message.trim() );
    console.log( 'Author\'s name', details.author.name );
    console.log( 'Author\'s email', details.author.email );
  });
});
