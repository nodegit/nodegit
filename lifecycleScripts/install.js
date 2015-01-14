var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var path = require("path");
var fs = require("fs");

var local = path.join.bind(path, __dirname);
var pkg = require(local("../package"));
var cp = require("child_process");

var checkPrepared = require(local("checkPrepared"));
var forNodeWebkit = require("for-node-webkit");
var prepareForBuild = require(local("prepareForBuild"));

var exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});


forNodeWebkit(local(".."))
  .then(function(results) {
    return results.nwVersion;
  })
  .then(function(nodeWebkitVersion) {
    if (nodeWebkitVersion) {
      console.info("[nodegit] Must build for node-webkit");
      return checkAndBuild(nodeWebkitVersion);
    }
    if (fs.existsSync(local("../.didntcomefromthenpmregistry"))) {
      checkAndBuild();
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
    return exec("node-pre-gyp build")
      .then(
        function() {
          console.info("[nodegit] Completed installation successfully.");
          return Promise.done();
        },
        function() {
          console.info("[nodegit] Failed to install prebuilt binary, building manually.");
          return checkAndBuild();
        }
      );
  })


function checkAndBuild(nwVersion) {
  console.info("[nodegit] Making sure dependencies are available and native code is generated");

  return checkPrepared.checkAll()
    .then(function(allGood) {
      if (allGood) {
        return build(nwVersion);
      }
      else {
        console.info("[nodegit] Something is missing, retrieving dependencies and regenerating code");
        return prepareForBuild()
          .then(function() {
            return build(nwVersion);
          });
      }
    });
}

function build(nwVersion) {
  console.info("[nodegit] Everything is ready to go, attempting compilation");
  if (nwVersion) {
    console.info("[nodegit] Building native node-webkit module.");
  }
  else {
    console.info("[nodegit] Building native node module.");
  }

  var opts = {
    cwd: local(".."),
    maxBuffer: Number.MAX_VALUE
  };

  var builder = nwVersion ? "nw-gyp" : "node-gyp";
  var target = (nwVersion ? "--target=\"" + nwVersion + "\"": "");
  var debug = (process.env.BUILD_DEBUG ? " --debug" : "");
  var cmd = path.resolve(".", "node_modules", ".bin", builder)
    + " clean configure"
    + debug
    + target
    + " build";

  return exec(cmd, opts)
    .then(function() {
      console.info("[nodegit] Compilation complete.");
      console.info("[nodegit] Completed installation successfully.");
    },
    function(err, stderr) {
      console.error(err);
      console.error(stderr);
    });
}
