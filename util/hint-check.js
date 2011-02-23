var nodejshint = require( './nodejshint.js' ).test;

nodejshint( [ 'lib/index.js', 'lib/ref.js', 'lib/repo.js', 'lib/error.js' ], function( failures ) {
  if( !failures ) {
    process.exit( 0 );
  } else {
    process.exit( 1 );
  }
});
