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
    var opensslDir = rooted("vendor/openssl/openssl");
    var newEnv = {};
    Object.keys(process.env).forEach(function(key) {
      newEnv[key] = process.env[key];
    });
    newEnv.CPPFLAGS = newEnv.CPPFLAGS || "";
    newEnv.CPPFLAGS += " -I" + path.join(opensslDir, "include");
    newEnv.CPPFLAGS = newEnv.CPPFLAGS.trim();

    cp.exec(
      rooted("vendor/libssh2/configure") +
        " --with-libssl-prefix=" + opensslDir,
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
