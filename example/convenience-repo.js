// Load in the module
var git = require( 'nodegit' );
// Open a repository for reading
git.repo( '../.git', function( err, repo ) {
  // Success is always 0, failure is always an error string
  if( err ) { throw err; }
  // Use the master branch
  repo.branch( 'master', function( err, branch ) {
    if( err ) { throw err; }
    // Iterate over the revision history
    branch.history.each( function( i, commit ) {
      // Print out `git log` emulation
      console.log( 'commit ' + commit.sha );
      console.log( commit.author.name + ' <' + commit.author.email + '>' );
      console.log( commit.time );
      console.log( commit.message );
    });
  });
});
