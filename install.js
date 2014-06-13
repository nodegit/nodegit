// Core Node.js modules.
var os = require('os');
var fs = require('fs');
var path = require('path');
var zlib = require('zlib');
var exec = require('child_process').exec;

// Third-party modules.
var Q = require('q');
var request = require('request');
var tar = require('tar');
var which = require('which');
var rimraf = require('rimraf');

// This will take in an object and find any matching keys in the environment
// to use as overrides.
//
// ENV variables:
//
// PKG: Location of `package.json` sans `.json`.
// LIBGIT2: Location of libgit2 source. 
// BUILD: Location of nodegit build directory.
function envOverride(obj) {
  // Look through all keys.
  return Object.keys(obj).reduce(function(obj, key) {
    var normalize = key.toUpperCase();

    // Check for process environment existence.
    if (normalize in process.env) {
      obj[key] = process.env[normalize];
    }

    return obj;
  }, obj);
}

// Convert to the correct system path.
function systemPath(parts) {
  return parts.join(path.sep);
}

// Will be used near the end to configure `node-gyp`.
var python, cmake;

// Common reusable paths that can be overwritten by environment variables.
var paths = envOverride({
  pkg: __dirname + '/package',
  libgit2: __dirname + '/vendor/libgit2/',
  build: __dirname + '/vendor/libgit2/build/',
  release: __dirname + '/build/Release'
});

// Load the package.json.
var pkg = require(paths.pkg);

// Ensure all dependencies are available.
var dependencies = Q.allSettled([
  // This will prioritize `python2` over `python`, because we always want to
  // work with Python 2.* if it's available.
  Q.nfcall(which, 'python2'),
  Q.nfcall(which, 'python'),

  // Check for any version of CMake.
  Q.nfcall(which, 'cmake'),
])

// Determine if all the dependency requirements are met.
.then(function(results) {
  console.info('[nodegit] Determining dependencies.');

  // Assign to reusable variables.
  python = results[0].value || results[1].value;
  cmake = results[2].value;
  
  // Now lets check the Python version to ensure it's < 3.
  return Q.nfcall(exec, python + ' --version').then(function(version) {
    if (version[1].indexOf('Python 3') === 0) {
      throw new Error('Incorrect version of Python, gyp requires < 3.');
    }
  });
})

// Successfully found all dependencies.  First step is to clean the vendor
// directory.
.then(function() {
  console.info('[nodegit] Removing vendor/libgit2.');

  return Q.ninvoke(rimraf, null, paths.libgit2);
})

// Now fetch the libgit2 source from GitHub.
.then(function() {
  console.info('[nodegit] Fetching vendor/libgit2.');

  var url = 'https://github.com/libgit2/libgit2/tarball/' + pkg.libgit2;
  
  var extract = tar.Extract({
    path: paths.libgit2,
    strip: true
  });

  // First extract from Zlib and then extract from Tar.
  var expand = request.get(url).pipe(zlib.createUnzip()).pipe(extract);

  return Q.ninvoke(expand, 'on', 'end');
})

// Configure the native module using node-gyp.
.then(function() {
  console.info('[nodegit] Configuring native node module.');

  return Q.nfcall(exec, systemPath([
    '.', 'node_modules', '.bin', 'node-gyp configure --python ' + python
  ]), {
    cwd: '.'
  });
})

// Build the native module using node-gyp.
.then(function() {
  console.info('[nodegit] Building native node module.');

  return Q.nfcall(exec, systemPath([
    '.', 'node_modules', '.bin', 'node-gyp build'
  ]), {
    cwd: '.',
    maxBuffer: Number.MAX_VALUE
  });
})

// Attempt to fallback on a prebuilt binary.
.fail(function(message) {
  console.info('[nodegit] Failed to build nodegit.');
  console.info('[nodegit] Attempting to fallback on a prebuilt binary.');

  function fetchPrebuilt() {
    console.info('[nodegit] Fetching binary from S3.');

    // Using the node-pre-gyp module, attempt to fetch a compatible build.
    return Q.nfcall(exec, 'node-pre-gyp install');
  }

  // Attempt to fetch prebuilt binary.
  return Q.ninvoke(fs, 'mkdir', paths.release)
    .then(fetchPrebuilt, fetchPrebuilt);
})

// Display a warning message about failing to build native node module.
.fail(function(message) {
  console.info('[nodegit] Failed to build and install nodegit.');
  console.info(message.message);
  console.info(message.stack);
})

// Display a success message.
.then(function() {
  console.info('[nodegit] Completed installation successfully.');
})

// Display a warning message about failing to build native node module.
.fail(function(message) {
  console.info('[nodegit] Failed to build nodegit.');
  console.info(message.message);
  console.info(message.stack);
});
