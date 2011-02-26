var nodejshint = require( './nodejshint.js' ).test,

files = [ 'lib/index.js', 'lib/ref.js', 'lib/repo.js', 'lib/error.js' ];

nodejshint( files, function( failures ) {
  if( !files.length ) {
    process.exit( 0 );
  } else {
    process.exit( 1 );
  }
});
