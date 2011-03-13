var nodejshint = require( './nodejshint.js' ).test,

files = [
  'lib/blob.js',
  'lib/commit.js',
  'lib/error.js',
  'lib/index.js',
  'lib/obj.js',
  'lib/oid.js',
  'lib/ref.js',
  'lib/repo.js',
  'lib/revwalk.js',
  'lib/sig.js',
  'lib/tree.js',
  'lib/tree_entry.js',
  'lib/util.js'
];

nodejshint( files, function( failures ) {
  if( !files.length ) {
    process.exit( 0 );
  }
  else {
    process.exit( 1 );
  }
});
