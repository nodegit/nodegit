var fs = require("fs");
var path = require("path");

var determineTarget = require("./determineTarget");
var prepareForBuild = require("./prepareForBuild");
var build = require("./build");
var exec = require("./exec");

var local = path.join.bind(path, __dirname);

var buildAction = "rebuild";

return determineTarget()
  .then(function(targetInfo) {
    if (fs.existsSync(local("../.didntcomefromthenpmregistry"))) {
      return prepareAndBuild(buildAction, targetInfo);
    }
    if (process.env.BUILD_DEBUG) {
      console.info("[nodegit] Doing a debug build, no fetching allowed.");
      return prepareAndBuild(buildAction, targetInfo);
    }
    if (process.env.BUILD_ONLY) {
      console.info("[nodegit] BUILD_ONLY is set to true, no fetching allowed.");
      return prepareAndBuild(buildAction, targetInfo);
    }
    return installPrebuilt(targetInfo);
  })
  .then(function() {
    console.info("[nodegit] Compilation complete.");
    console.info("[nodegit] Completed installation successfully.");
    process.exitCode = 0;
  },
  function(err, stderr) {
    console.error(err);
    console.error(stderr);
    process.exitCode = 13;
  });

function installPrebuilt(targetInfo) {
  console.info("[nodegit] Fetching binary from S3.");
  var installArguments = targetInfo.args.join(" ");
  return exec("node-pre-gyp install " + installArguments)
    .then(
      function() {
        console.info("[nodegit] Completed installation successfully.");
      },
      function(err) {
        console.info("[nodegit] Failed to install prebuilt binary, " +
          "building manually.");
        console.error(err);
        return prepareAndBuild(buildAction, targetInfo);
      }
    );
}

function prepareAndBuild(action, targetInfo) {
  console.info("[nodegit] Regenerating and configuring code");
  return prepareForBuild()
    .then(function() {
      console.info("[nodegit] Everything is ready to go, " +
                   "attempting compilation.");
      console.info("[nodegit] Building native " +
                   targetInfo.target + " module.");
      return build(buildAction, targetInfo);
    });
}
