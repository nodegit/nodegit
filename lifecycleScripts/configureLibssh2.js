var Promise = require("nodegit-promise");
var cp = require("child_process");
var path = require("path");
var rooted = path.join.bind(path, __dirname, "..");

module.exports = function retrieveExternalDependencies() {
  if (process.platform === "win32") {
    return Promise.resolve("");
  }

  return new Promise(function(resolve, reject) {
    console.info("[nodegit] Configuring libssh2.");
    cp.execFile(
      rooted("vendor/libssh2/") + "configure",
      {cwd: rooted("vendor/libssh2/")},
      function(err, stdout, stderr) {
        if (err) {
          console.info("[nodegit] Configuring libssh2 failed.");
          console.error(err);
          console.error(stderr);
          reject(err, stderr);
        }
        else {
          console.info("[nodegit] Configuring libssh2 succeeded.");
          resolve(stdout);
        }
      }
    );
  });
};

// Called on the command line
if (require.main === module) {
  if (process.platform === "win32") {
    console.log("nothing to do");
  }
  else {
    module.exports();
  }
}
