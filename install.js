var os = require("os");
var path = require("path");
var zlib = require("zlib");
var tar = require("tar");

var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var request = require("request");
var fse = promisify(require("fs-extra"));
var findParentDir = promisify(require('find-parent-dir'));
fse.ensureDir = promisify(fse.ensureDir, function() { return true; });

var exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});

var NODE_VERSION = Number(process.version.match(/^v(\d+\.\d+)/)[1]);

// If the build only flag is set.
var buildOnly = process.env.BUILD_ONLY;
var nodeWebkit = false;

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
var pythonPath = '/usr/bin/python';

var local = path.join.bind(path, __dirname);

// Common reusable paths that can be overwritten by environment variables.
var paths = envOverride({
  pkg: local("package"),
  libgit2: local("vendor/libgit2/"),
  libssh2: local("vendor/libssh2/"),
  http_parser: local("vendor/http_parser/"),
  sys: {
    include: local("include/sys/"),
    src: local("src/sys/"),
    build: local("build/Release/obj.target/src/sys/")
  },
  release: local("build/Release/")
});

// Load the package.json.
var pkg = require(paths.pkg);

if (NODE_VERSION === 0.1) {
  pkg.http_parser = pkg.http_parser["0.10"];
}

fse.ensureDir(path.resolve(__dirname, paths.release))
.then(detectNodeWebkit.call(null, __dirname))
.then(fetch)
.then(finish, compile)
.done()

function fetch() {
  console.info("[nodegit] Fetching binary from S3.");

  if (nodeWebkit) {
    throw new Error("Must build for node-webkit");
  }

  if (buildOnly) {
    throw new Error("BUILD_ONLY is set to true, no fetching allowed.");
  }

  // Using the node-pre-gyp module, attempt to fetch a compatible build.
  return exec("node-pre-gyp install");
}

function compile(err) {
  if (buildOnly || nodeWebkit) {
    console.info("[nodegit] " + err.message);
  }
  else  {
    console.info("[nodegit] Failed to install prebuilt, attempting compile.");
  }

  console.info("[nodegit] Determining dependencies.");

  return python()
    .then(getVendorLib("libgit2", "https://github.com/libgit2/libgit2/tarball/" + pkg.libgit2.sha))
    .then(getVendorLib("libssh2", pkg.libssh2.url))
    .then(getVendorLib("http_parser", pkg.http_parser.url))
    .then(buildNative)
    .then(finish, fail);

}

function python() {
  return exec("which python2")
    .then(function(which){
      return which;
    }, function(err) {
      return null;
    })
    .then(function(path) {
      return path || exec("which python");
    })
    .then(function(which) {
      return which;
    }, function(err) {
      return null;
    })
    .then(function(path) {
      pythonPath = path.trim();
      if (!pythonPath) {
        throw new Error("Python is required to build libgit2.");
      }
    })
    .then(function() {
      return exec(pythonPath + " -V 2>&1");
    })
    .then(function(version) {
      if (version[1].indexOf("Python 3") === 0) {
        throw new Error("Incorrect version of Python, gyp requires < 3.");
      }
    });
}

function getVendorLib(name, url) {
  return function() {
    var version = pkg[name].sha || pkg[name].version;
    console.info("[nodegit] Detecting vendor/" + name + ".");
    if (fse.existsSync(paths[name] + version)) {
      console.info("[nodegit] vendor/" + name + " already exists.");
      return new Promise(function(resolve, reject) {resolve() });
    }
    else {
      console.info("[nodegit] Removing outdated vendor/" + name + ".");
      return fse.remove(paths[name])
        .then(function() {
          return new Promise(function (resolve, reject) {

            console.info("[nodegit] Fetching vendor/" + name + ".");

            var extract = tar.Extract({
              path: paths[name],
              strip: true
            });

            request.get(url).pipe(zlib.createUnzip()).pipe(extract)
              .on("error", reject)
              .on("end", resolve);
          });
        }).then(function() {
          return fse.writeFile(paths[name] + version, "");
        }).then(function() {
          if ((name == "libssh2") && (process.platform !== "win32")) {

            return exec(paths[name] + "configure", {cwd: paths[name]});
          }
        });
    }
  }
}

function buildNative() {
  return exec("cd " + __dirname).then(function() {
    if (nodeWebkit) {
      console.info("[nodegit] Building native node-webkit module.");
    }
    else {
      console.info("[nodegit] Building native node module.");
    }

    var builder = nodeWebkit ? "nw-gyp" : "node-gyp";

    var cmd = path.resolve(".", "node_modules", ".bin", builder)
      + " clean configure "
      + (nodeWebkit ? "--target=\"" + nodeWebkit + "\"": "")
      + " build " + pythonFlag
      + "--python \"" + pythonPath + "\""

    var opts = {
      cwd: __dirname,
      maxBuffer: Number.MAX_VALUE
    };
    return exec(cmd, opts);
  })
}

function detectNodeWebkit(directory) {
  if (directory) {
    var pkg = require(path.resolve(directory, "package.json"));

    nodeWebkit = pkg.engines && pkg.engines["node-webkit"];

    return findParentDir(path.resolve(directory, ".."), "package.json")
        .then(detectNodeWebkit);
  }
  else {
    return Promise.resolve();
  }
}

function finish() {
  console.info("[nodegit] Completed installation successfully.");
  return Promise.resolve().done();
}

function fail(message) {
  console.info("[nodegit] Failed to build and install nodegit.");
  console.info(message.message);

  return Promise.resolve().done();
}
