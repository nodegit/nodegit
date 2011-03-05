#!/usr/bin/env node

require.paths.unshift( '../vendor' );

try {
  var reporter = require( '../vendor/nodeunit' ).reporters.default;
}
catch(e) {
  var sys = require( 'sys' );
  sys.puts( 'Cannot find nodeunit module.' );
  sys.puts( 'You can download submodules for this project by doing:' );
  sys.puts( '' );
  sys.puts( '    git submodule init vendor/nodeunit' );
  sys.puts( '    git submodule update vendor/nodeunit' );
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
    'raw-repo.js',
    'raw-oid.js',
    'raw-commit.js',
    'raw-error.js',
    
    // TODO:
    //'raw-revwalk.js',

    // Convenience API
    'convenience-repo.js'
  ]
);
