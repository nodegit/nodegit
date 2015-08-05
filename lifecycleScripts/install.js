var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var path = require("path");
var fs = require("fs");

var checkPrepared = require("./checkPrepared");
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
    if (nwVersion) {
      console.info("[nodegit] Must build for node-webkit/nw.js");
      return checkAndBuild();
    }
    else if (asVersion) {
      console.info("[nodegit] Must build for atom-shell");
      return checkAndBuild();
    }
    if (fs.existsSync(local("../.didntcomefromthenpmregistry"))) {
      return checkAndBuild();
    }
    if (process.env.BUILD_DEBUG) {
      console.info("[nodegit] Doing a debug build, no fetching allowed.");
      return checkAndBuild();
    }
    if (process.env.BUILD_ONLY) {
      console.info("[nodegit] BUILD_ONLY is set to true, no fetching allowed.");
      return checkAndBuild();
    }
    console.info("[nodegit] Fetching binary from S3.");
    return exec("node-pre-gyp install")
      .then(
        function() {
          console.info("[nodegit] Completed installation successfully.");
        },
        function() {
          console.info("[nodegit] Failed to install prebuilt binary, " +
            "building manually.");
          return checkAndBuild();
        }
      );
  });


function checkAndBuild() {
  console.info("[nodegit] Making sure dependencies are available and native " +
    "code is generated");

  return checkPrepared.checkAll()
    .then(function(allGood) {
      if (allGood) {
        return Promise.resolve();
      }
      else {
        console.info("[nodegit] Something is missing, retrieving " +
        "dependencies and regenerating code");
        return prepareForBuild();
      }
    })
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

  var prefix = "";
  var target = "";
  var debug = (process.env.BUILD_DEBUG ? " --debug" : "");
  var builder = "pangyp";
  var distUrl = "";

  if (asVersion) {
    prefix = (process.platform == "win32" ?
      "SET HOME=%HOME%\\.atom-shell-gyp&& " :
      "HOME=~/.atom-shell-gyp");

    target = "--target=" + asVersion;

    distUrl = "--dist-url=https://gh-contractor-zcbenz.s3." +
      "amazonaws.com/atom-shell/dist";
  }
  else if (nwVersion) {
    builder = "nw-gyp";
    target = "--target=" + nwVersion;
  }

  return exec("npm install --ignore-scripts")
    .then(function() {
      builder = path.resolve(".", "node_modules", ".bin", builder);
      builder = builder.replace(/\s/g, "\\$&");
      var cmd = [prefix, builder, "rebuild", target, debug, distUrl]
        .join(" ").trim();

      return exec(cmd, opts);
    })
    .then(function() {
      console.info("[nodegit] Compilation complete.");
      console.info("[nodegit] Completed installation successfully.");
    },
    function(err, stderr) {
      console.error(err);
      console.error(stderr);
    });
}
