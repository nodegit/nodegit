var async = require('async'),
    child_process = require('child_process'),
    spawn = child_process.spawn,
    path = require('path'),
    fs = require('fs'),
    request = require('request'),
    AdmZip = require('adm-zip');

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

    var libgit2ZipUrl = 'https://github.com/libgit2/libgit2/archive/v0.15.0.zip';
        zipFile = __dirname + '/vendor/libgit2.zip',
        unzippedFolderName = __dirname + '/vendor/libgit2-0.15.0',
        targetFolderName = __dirname + '/vendor/libgit2';

    async.series([
        function(callback) {
            request(libgit2ZipUrl)
                .pipe(fs.createWriteStream(zipFile))
                .on('close', function () {
                    callback();
            });

        }, function(callback) {
            var zip = new AdmZip(zipFile);
            zip.extractAllTo(__dirname + '/vendor/', true);
            fs.unlink(zipFile);
            callback();
        },
        function renameLibgit2Folder(callback) {
            fs.rename(unzippedFolderName, targetFolderName, callback);
        }
        ], function(error) {
            if (error) process.exit(error);
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
        // shpassthru('node-gyp configure --debug', callback);
        shpassthru('node-gyp configure', callback);
    },
    function(callback) {
        shpassthru('node-gyp build', callback);
    }
], function(err) {
    if(err) process.exit(err);
});
