var git = require("../");

var Odb = git.Odb;
var read = Odb.prototype.read;

Odb.prototype.read = function(oid, callback) {
  return read.call(this, oid).then(function(odbObject) {
    if (typeof callback === "function") {
      callback(null, odbObject);
    }

    return odbObject;
  }, callback);
};

module.exports = Odb;
