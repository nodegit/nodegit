var cp = require("child_process");
var fse = require("fs-extra");
var path = require("path");

const opensslVendorDirectory = path.resolve(__dirname, "..", "vendor", "openssl");
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
    var newEnv = {};
    Object.keys(process.env).forEach(function(key) {
      newEnv[key] = process.env[key];
    });

    let cpArgs = process.env.NODEGIT_OPENSSL_STATIC_LINK === '1'
      ? ` --with-libssl-prefix=${opensslVendorDirectory}`
      : '';
    cp.exec(
      `${libssh2ConfigureScript}${cpArgs}`,
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
