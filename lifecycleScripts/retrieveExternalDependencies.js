var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var fse = promisify("fs-extra");
var zlib = require("zlib");
var cp = require("child_process");
var path = require("path");

var local = path.join.bind(path, __dirname);
var rooted = path.join.bind(path, __dirname, "..");

var check = require(local("checkPrepared")).checkVendor;
var pkg = require(rooted("package"));
var tar;
var request;

var NODE_VERSION = Number(process.version.match(/^v(\d+\.\d+)/)[1]);

module.exports = function retrieveExternalDependencies() {
  tar = require("tar");
  request = require("request");
  return Promise.all([
    getVendorLib("libgit2"),
    getVendorLib("libssh2"),
    getVendorLib("http_parser")
  ]);
};


function getVendorLib(name) {
  var vendorPath = "vendor/" + name + "/";
  var vendorPackage = pkg[name];
  if (NODE_VERSION === 0.1 && vendorPackage["0.10"]) {
    vendorPackage = vendorPackage["0.10"];
  }

  var version = vendorPackage.sha || vendorPackage.version;

  console.info("[nodegit] Detecting " + vendorPath + version + ".");
  return check(name)
    .then(function(exists) {
      if (exists) {
        console.info("[nodegit] " + vendorPath + version + " already exists.");
        return Promise.resolve();
      }
      else {
        return check(name, true)
          .then(function(exists) {
            if (exists) {
              console.info("[nodegit] Removing outdated " + vendorPath + ".");
              return fse.remove(rooted(vendorPath));
            }
            else {
              console.info("[nodegit] " + vendorPath + " not found.");
              return Promise.resolve();
            }
          })
          .then(function() {
            return new Promise(function (resolve, reject) {
              console.info("[nodegit] Retrieving " + vendorPath + ".");

              var extract = tar.Extract({
                path: rooted("vendor/" + name + "/"),
                strip: true
              });

              request.get(vendorPackage.url)
                .pipe(zlib.createUnzip())
                .pipe(extract)
                .on("error", reject)
                .on("end", resolve);
            });

          })
          .then(function() {
            return fse.writeFile(rooted(vendorPath + version), "");
          })
          .then(function() {
            if ((name == "libssh2") && (process.platform !== "win32")) {
              return new Promise(function(resolve, reject) {
                console.info("[nodegit] Configuring libssh2.");
                cp.execFile(
                  rooted(vendorPath) + "configure",
                  {cwd: rooted(vendorPath)},
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
            }
            else {
              return Promise.resolve();
            }
          })
          .then(function() {
            console.info("[nodegit] Successfully updated " + vendorPath +
              version + ".");
          });
      }
    });

}

// Called on the command line
if (require.main === module) {
  module.exports();
}
