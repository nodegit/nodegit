var cp = require("child_process");
var fse = require('fs-extra');
var path = require("path");

const libssh2VendorDirectory = path.resolve(__dirname, "..", "vendor", "libssh2");
const libssh2ConfigureScript = path.join(libssh2VendorDirectory, "configure");
const libssh2StaticConfigDirectory  = path.resolve(__dirname, "..", "vendor", "static_config", "libssh2");

module.exports = function retrieveExternalDependencies() {
  console.info("[nodegit] Configuring libssh2.");

  // Copy Windows / Mac preconfigured files
  if (process.platform === "win32" || process.platform === "darwin") {
    return fse.copy(
      path.join(libssh2StaticConfigDirectory, process.platform),
      path.join(libssh2VendorDirectory, "src")
    );
  }

  // Run the `configure` script on Linux
  return new Promise(function(resolve, reject) {

    var opensslDir = process.argv[2];
    var isElectron = process.argv[3] === "1";
    var opensslIncludes = isElectron ? path.join(opensslDir, "includes") : opensslDir;

    var newEnv = {};
    Object.keys(process.env).forEach(function(key) {
      newEnv[key] = process.env[key];
    });

    newEnv.CPPFLAGS = newEnv.CPPFLAGS || "";
    newEnv.CPPFLAGS += ` -I${opensslIncludes}`;
    newEnv.CPPFLAGS = newEnv.CPPFLAGS.trim();

    var maybeLibsslPrefix = "";
    if (isElectron) {
      maybeLibsslPrefix = ` --with-libssl-prefix=${opensslDir}`;
    }

    cp.exec(
      libssh2ConfigureScript + maybeLibsslPrefix,
      {
        cwd: libssh2VendorDirectory,
        env: newEnv
      },
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
  module.exports();
}
