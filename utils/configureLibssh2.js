var cp = require("child_process");
var fse = require("fs-extra");
var path = require("path");

const { hostArch, targetArch } = require("./buildFlags");

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
  let cpArgs = ` --with-libssl-prefix=${opensslVendorDirectory}`;

  const archConfigMap = {
    'x64': 'x86_64-linux-gnu',
    'arm64': 'aarch64-linux-gnu'
  };

  cpArgs += ` --build=${archConfigMap[hostArch]}`;
  cpArgs += ` --host=${archConfigMap[targetArch]}`;

  return new Promise(function(resolve, reject) {
    cp.exec(
      `${libssh2ConfigureScript}${cpArgs}`,
      {
        cwd: libssh2VendorDirectory
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
