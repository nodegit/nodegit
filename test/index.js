require.paths.unshift( '../vendor' );

try {
  var reporter = require( '../vendor/nodeunit' ).reporters['default'];
}
catch( e ) {
  var sys = require( 'sys' );
  sys.puts( 'Cannot find nodeunit module.' );
  sys.puts( 'You can download submodules for this project by doing:' );
  sys.puts( '' );
  sys.puts( '    git submodule update --init' );
  sys.puts( '' );
  process.exit();
}

try {
  var rimraf = require( '../vendor/rimraf' );
}
catch(e) {
  var sys = require( 'sys' );
  sys.puts( 'Cannot find rimraf module.' );
  sys.puts( 'You can download submodules for this project by doing:' );
  sys.puts( '' );
  sys.puts( '    git submodule init vendor/rimraf' );
  sys.puts( '    git submodule update vendor/rimraf' );
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
