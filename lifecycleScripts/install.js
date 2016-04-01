var promisify = require("promisify-node");
var path = require("path");
var fs = require("fs");
var cp = require("child_process");
var prepareForBuild = require("./prepareForBuild");

var exec = promisify(function(command, opts, callback) {
  return cp.exec(command, opts, callback);
});

var fromRegistry;
try {
  fs.statSync(path.join(__dirname, "..", "include"));
  fs.statSync(path.join(__dirname, "..", "src"));
  fromRegistry = true;
}
catch(e) {
  fromRegistry = false;
}

if (!fromRegistry) {
  console.info("[nodegit] Local install, no fetching allowed.");
  return prepareAndBuild();
}
if (process.env.BUILD_DEBUG) {
  console.info("[nodegit] Doing a debug build, no fetching allowed.");
  return prepareAndBuild();
}
if (process.env.BUILD_ONLY) {
  console.info("[nodegit] BUILD_ONLY is set to true, no fetching allowed.");
  return prepareAndBuild();
}

return installPrebuilt();

function installPrebuilt() {
  console.info("[nodegit] Fetching binary from S3.");
  var npg = pathForTool("node-pre-gyp");
  return exec("\""+ npg + "\" install --fallback-to-build=false")
    .then(
      function() {
        console.info("[nodegit] Completed installation successfully.");
      },
      function(err) {
        console.info("[nodegit] Failed to install prebuilt binary:");
        console.error(err);
        console.info("[nodegit] Building manually. (You'll be here a while.)");
        return prepareAndBuild();
      }
    );
}

function pathForTool(name) {
  var toolPath = path.resolve(".", "node_modules", ".bin", name);
  if (process.platform == "win32") {
    toolPath += ".cmd";
  }
  return toolPath;
}

function prepareAndBuild() {
  console.info("[nodegit] Regenerating and configuring code");
  return prepareForBuild()
    .then(function() {
      return build();
    });
}

function build() {
  console.info("[nodegit] Everything is ready to go, attempting compilation");

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
    }
    else if (~arg.indexOf("nsjwVersion")) {
      nwjsVersion = arg.split("=")[1].trim();
    }
  });

  if (electronVersion) {
    target = "--target=" + electronVersion;
    distUrl = "--dist-url=https://atom.io/download/atom-shell";
    runtime = "--runtime=electron";
  }
  else if (nwjsVersion) {
    builder = "nw-gyp";
    target = "--target=" + nwjsVersion;
    runtime = "--runtime=node-webkit";
  }

  var home = process.platform == "win32" ?
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

  console.log(args);
  return new Promise(function(resolve, reject) {
    var child = cp.spawn(cmd, args, opts);
    child.on("close", function(code) {
      console.log(code);
      if (code) {
        reject(code);
        process.exitCode = 13;
      }
      else {
        resolve();
      }
    });
  });
}
