var path = require("path");
var fs = require("fs");
var cp = require("child_process");
var prepareForBuild = require("./prepareForBuild");
var exec = require("../utils/execPromise");

var fromRegistry;

try {
  fs.statSync(path.join(__dirname, "..", "include"));
  fs.statSync(path.join(__dirname, "..", "src"));
  fs.statSync(path.join(__dirname, "..", "lib"));
  fs.statSync(path.join(__dirname, "..", "utils"));
  fromRegistry = true;
} catch (e) {
  fromRegistry = false;
}

function pathForTool(name) {
  var toolPath = path.resolve(".", "node_modules", ".bin", name);
  if (process.platform === "win32") {
    toolPath += ".cmd";
  }
  return toolPath;
}

function build() {
  /* eslint-disable no-console */
  console.info("[nodegit] Everything is ready to go, attempting compilation");
  /* eslint-enable */

  var electronVersion = process.env.ELECTRON_VERSION;
  var nwjsVersion = process.env.NWJS_VERSION;
  var opts = {
    cwd: ".",
    maxBuffer: Number.MAX_VALUE,
    env: process.env,
    stdio: "inherit"
  };

  var builder = "node-gyp";
  var debug = (process.env.BUILD_DEBUG ? "--debug" : "");
  var target = "";
  var arch = (process.env.TARGET_ARCH ?
     "--arch=" + process.env.TARGET_ARCH : "");
  var distUrl = "";
  var runtime = "";

  process.argv.forEach(function(arg) {
    if (~arg.indexOf("electronVersion")) {
      electronVersion = arg.split("=")[1].trim();
    } else if (~arg.indexOf("nsjwVersion")) {
      nwjsVersion = arg.split("=")[1].trim();
    }
  });

  if (electronVersion) {
    target = "--target=" + electronVersion;
    distUrl = "--dist-url=https://atom.io/download/atom-shell";
    runtime = "--runtime=electron";
  } else if (nwjsVersion) {
    builder = "nw-gyp";
    target = "--target=" + nwjsVersion;
    runtime = "--runtime=node-webkit";
  }

  var home = process.platform === "win32" ?
    process.env.USERPROFILE : process.env.HOME;

  opts.env.HOME = path.join(home, ".nodegit-gyp");

  var cmd = pathForTool(builder);
  var args = [
    "rebuild",
    debug,
    target,
    arch,
    distUrl,
    runtime
  ]
  .filter(function(arg) {
    return arg;
  });

  return new Promise(function(resolve, reject) {
    var child = cp.spawn(cmd, args, opts);
    child.on("close", function(code) {
      console.log(code);  // eslint-disable-line no-console
      if (code) {
        reject(code);
        process.exitCode = 13;
      } else {
        resolve();
      }
    });
  });
}

function transpileJavascript() {
  var cmd = pathForTool("babel");
  var args = [
    "--presets",
    "es2015",
    "-d",
    "./lib",
    "./js/lib"
  ];
  var opts = {
    cwd: ".",
    maxBuffer: Number.MAX_VALUE,
    env: process.env,
    stdio: "inherit"
  };
  var home = process.platform === "win32" ?
    process.env.USERPROFILE : process.env.HOME;

  opts.env.HOME = path.join(home, ".nodegit-gyp");

  return new Promise(function(resolve, reject) {
    var child = cp.spawn(cmd, args, opts);
    child.on("close", function(code) {
      if (code) {
        reject(code);
        process.exitCode = 13;
      } else {
        resolve();
      }
    });
  });
}

function prepareAndBuild() {
  /* eslint-disable no-console */
  console.info("[nodegit] Regenerating and configuring code");
  /* eslint-enable */
  return prepareForBuild()
    .then(function() {
      return build();
    })
    .then(function() {
      return transpileJavascript();
    });
}

/* eslint-disable no-console */
function installPrebuilt() {
  console.info("[nodegit] Fetching binary from S3.");
  var npg = pathForTool("node-pre-gyp");
  return exec("\"" + npg + "\" install --fallback-to-build=false")
    .then(
      function() {
        console.info("[nodegit] Compvared installation successfully.");
      },
      function(err) {
        console.info("[nodegit] Failed to install prebuilt binary:");
        console.error(err);
        console.info("[nodegit] Building manually. (You'll be here a while.)");
        return prepareAndBuild();
      }
    );
}

if (!fromRegistry) {
  console.info("[nodegit] Local install, no fetching allowed.");
  prepareAndBuild();
} else if (process.env.BUILD_DEBUG) {
  console.info("[nodegit] Doing a debug build, no fetching allowed.");
  prepareAndBuild();
} else if (process.env.BUILD_ONLY) {
  console.info("[nodegit] BUILD_ONLY is set to true, no fetching allowed.");
  prepareAndBuild();
} else {
  installPrebuilt();
}
/* eslint-enable */
