var promisify = require("promisify-node");
var path = require("path");
var fs = require("fs");

var whichNativeNodish = require("which-native-nodish");
var prepareForBuild = require("./prepareForBuild");

var exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});
var nwVersion = null;
var asVersion = null;

var local = path.join.bind(path, __dirname);

return whichNativeNodish("..")
  .then(function(results) {
    nwVersion = results.nwVersion;
    asVersion = results.asVersion;
  })
  .then(function() {
    if (fs.existsSync(local("../.didntcomefromthenpmregistry"))) {
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
    var args = [];
    if (asVersion) {
      args.push("--runtime=electron");
      args.push("--target=" + asVersion);
    } else if (nwVersion) {
      args.push("--runtime=node-webkit");
      args.push("--target=" + nwVersion);
    }
    return installPrebuilt(args);
  });

function installPrebuilt(args) {
  console.info("[nodegit] Fetching binary from S3.");
  var installArguments = args.join(" ");
  return exec("node-pre-gyp install " + installArguments)
    .then(
      function() {
        console.info("[nodegit] Completed installation successfully.");
      },
      function(err) {
        console.info("[nodegit] Failed to install prebuilt binary, " +
          "building manually.");
        console.error(err);
        return prepareAndBuild();
      }
    );
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
  if (nwVersion) {
    console.info("[nodegit] Building native node-webkit module.");
  }
  else {
    console.info("[nodegit] Building native node module.");
  }

  var opts = {
    cwd: ".",
    maxBuffer: Number.MAX_VALUE,
    env: process.env
  };

  var target = "";
  var debug = (process.env.BUILD_DEBUG ? " --debug" : "");
  var builder = "node-pre-gyp";
  var distUrl = "";
  var runtime = "";

  if (asVersion) {
    var home = process.platform == "win32" ?
            process.env.USERPROFILE : process.env.HOME;

    opts.env.HOME = path.join(home, ".atom-shell-gyp");

    runtime = "--runtime=electron";

    target = "--target=" + asVersion;

    distUrl = "--dist-url=https://gh-contractor-zcbenz.s3." +
      "amazonaws.com/atom-shell/dist";
  }
  else if (nwVersion) {
    runtime = "--runtime=node-webkit";
    target = "--target=" + nwVersion;
  }

  builder = path.resolve(".", "node_modules", ".bin", builder);
  builder = builder.replace(/\s/g, "\\$&");
  var cmd = [
      builder, "rebuild", runtime, target,
      debug, distUrl, "--build-from-source"
    ]
    .join(" ").trim();

  return exec(cmd, opts)
    .then(function() {
        console.info("[nodegit] Compilation complete.");
        console.info("[nodegit] Completed installation successfully.");
        process.exitCode = 0;
      },
      function(err, stderr) {
        console.error(err);
        console.error(stderr);
        process.exitCode = 13;
      }
    );
}
