var os = require("os");
var path = require("path");
var zlib = require("zlib");
var tar;
var request;

var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));
var findParentDir = promisify(require('find-parent-dir'));

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
var pythonPath = "";

var local = path.join.bind(path, __dirname);

// Common reusable paths that can be overwritten by environment variables.
var paths = envOverride({
  pkg: local("package"),
  libgit2: local("vendor/libgit2/"),
  libssh2: local("vendor/libssh2/"),
  http_parser: local("vendor/http_parser/"),
  release: local("build/Release/"),
});

// Load the package.json.
var pkg = require(paths.pkg);

if (NODE_VERSION === 0.1) {
  pkg.http_parser = pkg.http_parser["0.10"];
}

// ensureDir doesn't promisify correctly so right now just use the callback
fse.ensureDir(path.resolve(__dirname, paths.release), function(err) {
  if (err) {
    console.log(err);
  }
  else {
    detectNodeWebkit.call(null, __dirname)
    .then(fetch)
    .then(finish, compile);
  }
});

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

  console.info("[nodegit] Installing all devDependencies");
  return exec("npm install --ignore-scripts --dont-prepublish")
    .then(function() {
      console.info("[nodegit] Determining source dependencies.");

      tar = require("tar");
      request = require("request");

      return Promise.all([
        python(),
        getVendorLib("libgit2", "https://github.com/libgit2/libgit2/tarball/" + pkg.libgit2.sha),
        getVendorLib("libssh2", pkg.libssh2.url),
        getVendorLib("http_parser", pkg.http_parser.url),
        guardGenerated()
      ]);
    })
    .then(buildNative)
    .then(finish, fail);
}

function python() {
  var pathFinderCommand = process.platform === "win32" ? "where" : "which";

  return exec(pathFinderCommand + " python2")
    .then(function(which){
      return which;
    }, function(err) {
      return null;
    })
    .then(function(path) {
      return path || exec(pathFinderCommand + " python");
    })
    .then(function(path) {
      return path;
    }, function(err) {
      return null;
    })
    .then(function(path) {
      if (!path) {
        throw new Error("Python is required to build libgit2.");
      }
      return path.trim();
    }, function(err) {
      throw new Error("Error finding python.");
    })
    .then(function(path) {
      pythonPath = path;
      return exec(path + " -V 2>&1");
    })
    .then(function(version) {
      if (version.trim().indexOf("Python 3") === 0) {
        throw new Error("Incorrect version of Python, gyp requires < 3.");
      }
    });
}

function getVendorLib(name, url) {
  var version = pkg[name].sha || pkg[name].version;
  console.info("[nodegit] Detecting vendor/" + name + ".");
  if (fse.existsSync(paths[name] + version)) {
    console.info("[nodegit] vendor/" + name + " already exists.");
    return Promise.resolve();
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

function guardGenerated() {
  return Promise.all([
    fse.stat(path.resolve(__dirname, "src/")),
    fse.stat(path.resolve(__dirname, "include/"))
  ]).then(function() {
    return Promise.resolve();
  }, function() {
    console.info("[nodegit] C++ files not found, generating now.");
    return exec("node generate");
  });
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
      + " build --python \"" + pythonPath + "\""

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
  if (!buildOnly) {
    console.info("[nodegit] Cleaning up");
    return Promise.all([
      fse.remove(path.resolve(__dirname, "src")),
      fse.remove(path.resolve(__dirname, "include")),
      fse.remove(path.resolve(__dirname, "generate/output")),
      fse.remove(path.resolve(__dirname, paths.libgit2)),
      fse.remove(path.resolve(__dirname, paths.libssh2)),
      fse.remove(path.resolve(__dirname, paths.http_parser))
      // exec("npm prune --production")
    ]).done();
  }
  else {
    return Promise.resolve().done();
  }
}

function fail(message) {
  console.info("[nodegit] Failed to build and install nodegit.");
  console.info(message.message);

  return Promise.resolve().done();
}
