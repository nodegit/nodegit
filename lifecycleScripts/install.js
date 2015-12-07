var fs = require("fs");
var path = require("path");
var promisify = require("promisify-node");
var prepareForBuild = require("./prepareForBuild");

var exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});

function install(type) {
  var helperPath = path.resolve(".", "node_modules", ".bin",
                                "node-pre-gyp");
  var cmd = "node " + helperPath + " install " + type;
  return exec(cmd);
}

function buildFromSource() {
  return prepareForBuild().then(function() {
    return install("--build-from-source");
  });
}

var fromRegistry;
try {
  fs.statSync(path.join(__dirname, '..', 'include'));
  fs.statSync(path.join(__dirname, '..', 'src'));
  fromRegistry = true;
}
catch(e) {
  fromRegistry = false;
}


if (fromRegistry) {
  // If we're developing, or installing via npm-git then we always want to build from source.
  return buildFromSource();
} else {
  // Don't use node-pre-gyp's built-in fallback. We need to prepareForBuild()
  // first.
  return install("--fallback-to-build=false").catch(buildFromSource);
}
