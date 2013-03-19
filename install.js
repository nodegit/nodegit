var async = require('async'),
    child_process = require('child_process'),
    spawn = child_process.spawn,
    path = require('path'),
    fs = require('fs'),
    request = require('request'),
    zlib = require('zlib'),
    tar = require('tar');

function passthru() {
    var args = Array.prototype.slice.call(arguments);
    var cb = args.splice(-1)[0];
    var cmd = args.splice(0, 1)[0];
    var opts = {};
    if(typeof(args.slice(-1)[0]) === 'object') {
        opts = args.splice(-1)[0];
    }
    var child = spawn(cmd, args, opts);

    child.stdout.pipe(process.stdout);
    child.stderr.pipe(process.stderr);
    child.on('exit', cb);
}

function shpassthru() {
    var cmd =
    passthru.apply(null, ['/bin/sh', '-c'].concat(Array.prototype.slice.call(arguments)));
}

function envpassthru() {
    passthru.apply(null, ['/usr/bin/env'].concat(Array.prototype.slice.call(arguments)));
}

var updateSubmodules = function(mainCallback) {
    console.log('[nodegit] Downloading libgit2 dependency.');
    async.series([
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

var buildDir = path.join(__dirname, 'vendor/libgit2/build');
async.series([
    function(callback) {
        // Check for presence of .git folder
        fs.exists(__dirname + '/.git', function(exists) {
            if (exists) {
                updateSubmodules(callback);
            } else {
                checkoutDependencies(callback);
            }
        });
    },
    function(callback) {
        console.log('[nodegit] Building libgit2 dependency.');
        envpassthru('mkdir', '-p', buildDir, callback);
    },
    function(callback) {
        envpassthru('cmake', '-DTHREADSAFE=1', '-DBUILD_CLAR=0', '..', {
            cwd: buildDir
        }, callback);
    },
    function(callback) {
        envpassthru('cmake', '--build', '.', {
            cwd: buildDir
        }, callback);
    },
    function(callback) {
        console.log('[nodegit] Building native module.');
        // shpassthru('node-gyp configure --python python2 --debug', callback);
        shpassthru('node-gyp configure --python python2', callback);
    },
    function(callback) {
        shpassthru('node-gyp build', callback);
    }
], function(err) {
    if(err) process.exit(err);
});
