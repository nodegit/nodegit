var cp = require("child_process");
var path = require("path");
var rooted = function (dir) {
  var fullPath = path.join(__dirname, "..", dir);
  var escapedPathForShell = fullPath.replace(/ /g, "\\ ");
  return escapedPathForShell;
};

module.exports = function retrieveExternalDependencies() {
  if (process.platform === "win32") {
    return Promise.resolve("");
  }

  return new Promise(function(resolve, reject) {
    console.info("[nodegit] Configuring libssh2.");
    var opensslDir = process.argv[2];
    var isElectron = process.argv[3] === "1";
    var opensslIncludes = isElectron ? path.join(opensslDir, "includes") : opensslDir;

    var newEnv = {};

    // For some magic reason, MacOS fails to build if we copy over the current environment variables, but Linux
    // builds fail if we DON'T copy over the current environment variables
    if (process.platform !== "darwin") {
      Object.keys(process.env).forEach(function(key) {
        newEnv[key] = process.env[key];
      });
    }

    newEnv.CPPFLAGS = newEnv.CPPFLAGS || "";
    newEnv.CPPFLAGS += ` -I${opensslIncludes}`;
    newEnv.CPPFLAGS = newEnv.CPPFLAGS.trim();

    var maybeLibsslPrefix = "";
    if (isElectron) {
      maybeLibsslPrefix = ` --with-libssl-prefix=${opensslDir}`;
    }

    cp.exec(
      rooted("vendor/libssh2/configure") + maybeLibsslPrefix,
      {cwd: rooted("vendor/libssh2/"), env: newEnv},
      function(err, stdout, stderr) {
        if (err) {
          console.error(err);
          console.error(stderr);
          reject(err, stderr);
        }
        else {
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
