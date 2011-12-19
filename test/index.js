
try {
  var reporter = require('nodeunit').reporters['default'];
}
catch( e ) {
  var sys = require( 'util' );
  sys.puts( 'Cannot find nodeunit module.' );
  sys.puts( 'You can install it by running:');
  sys.puts( '' );
  sys.puts( '    npm install');
  sys.puts( '' );
  process.exit();
}

try {
  var rimraf = require('rimraf');
}
catch(e) {
  var sys = require( 'util' );
  sys.puts( 'Cannot find rimraf module.' );
  sys.puts( 'You can install it by running:');
  sys.puts( '' );
  sys.puts( '    npm install');
  sys.puts( '' );
  process.exit();
}

process.chdir( './test' );
reporter.run(
  [
    // Raw API
    'raw-blob.js'
  , 'raw-commit.js'
  , 'raw-error.js'
  , 'raw-object.js'
  , 'raw-oid.js'
  , 'raw-reference.js'
  , 'raw-repo.js'
  , 'raw-revwalk.js'
    // Sig
    // Tree
    // Tree Entry
    // Util
    
    // Convenience API
  , 'convenience-repo.js'
    // Blob
    // Commit
    // Error
    // Obj
    // Oid
    // Ref
    // RevWalk
    // Sig
    // Tree
    // TreeEntry
  ]
);
