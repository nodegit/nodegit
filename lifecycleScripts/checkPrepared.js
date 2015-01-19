var Promise = require("nodegit-promise");
var path = require("path");
var fs = require("fs");
var rooted = path.join.bind(path, __dirname, "..");
var pkg = require(rooted("package"));

var NODE_VERSION = Number(process.version.match(/^v(\d+\.\d+)/)[1]);

module.exports.checkAll = function checkAll() {
  return Promise.all([
    checkVendor("libgit2"),
    checkVendor("libssh2"),
    checkVendor("http_parser"),

    checkExists("lib/nodegit.js"),
    checkExists("lib/enums.js"),
    checkExists("src"),
    checkExists("include")
  ]).then(function(checks) {
    return checks.reduce(function(soFar, currentCheck) {
      return soFar && currentCheck;
    }, true);
  });
};

function checkExists(name) {
  return new Promise(function(resolve, reject) {
    fs.exists(rooted(name), function(exists) {
      resolve(exists);
    });
  });
}
module.exports.checkExists = checkExists;

function checkVendor(name, skipVersion) {
  var version = "";
  if (!skipVersion) {
    var vendorPackage = pkg[name];
    if (NODE_VERSION === 0.1 && vendorPackage["0.10"]) {
      vendorPackage = vendorPackage["0.10"];
    }
    version = vendorPackage.sha || vendorPackage.version;
  }

  var vendorPath = "vendor/" + name + "/" + version;
  return checkExists(vendorPath);
}
module.exports.checkVendor = checkVendor;

function checkGenerated() {
  return Promise.all([
    checkExists("lib/nodegit.js"),
    checkExists("lib/enums.js"),
    checkExists("src"),
    checkExists("include")
  ]).then(function(checks) {
    return checks.reduce(function(soFar, currentCheck) {
      return soFar && currentCheck;
    }, true);
  });
}
module.exports.checkGenerated = checkGenerated;

// Called on the command line
if (require.main === module) {
  module.exports.checkAll();
}
