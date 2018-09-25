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
    var opensslDir;

    if (process.platform === "darwin") {
      opensslDir = "vendor/openssl";
    }

    var newEnv = {};
    Object.keys(process.env).forEach(function(key) {
      newEnv[key] = process.env[key];
    });

    var maybeLibsslPrefix = "";
    if (opensslDir) {
      newEnv.CPPFLAGS = newEnv.CPPFLAGS || "";
      newEnv.CPPFLAGS += " -I" + path.join(opensslDir, "include");
      newEnv.CPPFLAGS = newEnv.CPPFLAGS.trim();
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
    module.exports().done();
  }
}
