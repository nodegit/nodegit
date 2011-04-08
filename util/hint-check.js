var nodejshint = require( './nodejshint.js' ).test,

files = [
  // Test convenience api
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
  'lib/util.js',

  // Test unit test
  'test/convenience-repo.js',
  'test/index.js',
  'test/raw-blob.js',
  'test/raw-commit.js',
  'test/raw-error.js',
  'test/raw-obj.js',
  'test/raw-oid.js',
  'test/raw-ref.js',
  'test/raw-repo.js',
  'test/raw-revwalk.js',

  // Test examples
  'example/convenience-repo.js',
  'example/convenience-tree.js',
  'example/raw-error.js',
  'example/raw-oid.js',
  'example/raw-repo.js',
  'example/raw-revwalk.js'
];

nodejshint( files, function( failures ) {
  if( !files.length ) {
    process.exit( 0 );
  }
  else {
    process.exit( 1 );
  }
});
