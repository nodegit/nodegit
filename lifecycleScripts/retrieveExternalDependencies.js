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

module.exports = function retrieveExternalDependencies() {
  tar = require("tar");
  request = require("request");
  return Promise.all([
    getVendorLib("libgit2", "https://github.com/libgit2/libgit2/tarball/" + pkg.libgit2.sha),
    getVendorLib("libssh2", pkg.libssh2.url),
    getVendorLib("http_parser", pkg.http_parser.url)
  ])
};


function getVendorLib(name, url) {
  var vendorPath = "vendor/" + name + "/";
  var version = pkg[name].sha || pkg[name].version;

  console.info("[nodegit] Detecting " + vendorPath + ".");
  return check(name)
    .then(function(exists) {
      if (exists) {
        console.info("[nodegit] " + vendorPath + " already exists.");
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

              request.get(url)
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
                cp.exec(rooted(vendorPath) + "configure", {cwd: rooted(vendorPath)}, function(err, stdout, stderr) {
                  if (err) {
                    console.error(err);
                    console.error(stderr);
                    reject(err, stderr);
                  }
                  else {
                    resolve(stdout);
                  }
                })
              });
            }
            else {
              return Promise.resolve();
            }
          })
          .then(function() {
            console.info("[nodegit] Successfully updated " + vendorPath + ".");
          });
      }
    });

}

// Called on the command line
if (require.main === module) {
  module.exports();
}
