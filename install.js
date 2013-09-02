// Core Node.js modules.
var fs = require('fs');
var path = require('path');
var zlib = require('zlib');

// Third-party modules.
var Q = require('q');
var request = require('request');
var tar = require('tar');
var which = require('which');

// Build options.
var options = {
  // Normalize the libgit2 build directory.
  libgit2build: path.join(__dirname, 'vendor/libgit2/build')
};

function shpassthru() {
    passthru.apply(null, ['/bin/sh', '-c'].concat(Array.prototype.slice.call(arguments)));
}

function envpassthru() {
    passthru.apply(null, ['/usr/bin/env'].concat(Array.prototype.slice.call(arguments)));
}

var updateSubmodules = function(mainCallback) {
    console.log('[nodegit] Downloading libgit2 dependency.');
    async.series([26
        function(callback) {
            envpassthru('git', 'submodule', 'init', callback);
        }, function(callback) {
            envpassthru('git', 'submodule', 'update', callback);
        }
        ], function(error) {
            if (error) process.exit(error);
            mainCallback();
        });
};

var checkoutDependencies = function(mainCallback) {
    console.log('[nodegit] Downloading libgit2 dependency.');
    var commit = 'e953c1606d0d7aea680c9b19db0b955b34ae63c2';

    var url = 'https://github.com/libgit2/libgit2/tarball/'+ commit;
    var path = __dirname + '/vendor/libgit2/';
    request({
        url: url
    }).pipe(zlib.createUnzip()).pipe(tar.Extract({
        path: path,
        strip: true
    })).on('end', function() {
        mainCallback();
    });
};

Q.ninvoke(which, "python2").

async.series([
    function checkPython2Exists(callback) {
        exec('which python2',
          function (error) {
            if (!error) {
                pythonExecutable = 'python2';
                callback();
                return;
            }
            // python2 is not available, check for python
            exec('which python', function(error) {
                if (error) {
                    throw new Error('Python is required to build libgit2');
                }
                callback();
            });
        });

    },
    function prepareLibgit2Repository(callback) {
        // Check for presence of .git folder
        fs.exists(__dirname + '/.git', function(exists) {
            if (exists) {
                updateSubmodules(callback);
            } else {
                checkoutDependencies(callback);
            }
        });
    },
    function deleteExistingLibgit2BuildFolder(callback) {
        // fs.exists(libgit2BuildDirectory, function(exists) {
        //     if (exists) {
        //         fs.remove(libgit2BuildDirectory, callback);
        //     } else {
                callback();
            // }
        // });
    },
    function createLibgit2BuildDirectory(callback) {
        console.log('[nodegit] Building libgit2 dependency.');
        fs.mkdirs(libgit2BuildDirectory, callback);
    },
    function configureLibgit2(callback) {
        envpassthru('cmake', '-DTHREADSAFE=1', '-DBUILD_CLAR=0', '..', {
            cwd: libgit2BuildDirectory
        }, callback);
    },
    function buildLibgit2(callback) {
        envpassthru('cmake', '--build', '.', {
            cwd: libgit2BuildDirectory
        }, callback);
    },
    function configureNodegit(callback) {
        console.log('[nodegit] Building native module.');
        // shpassthru('node-gyp configure --python python2 --debug', callback);
        shpassthru('node-gyp configure --python ' + pythonExecutable, callback);
    },
    function buildNodegit(callback) {
        shpassthru('node-gyp build', callback);
    }
], function handleError(error) {
    if(error) process.exit(error);
});
