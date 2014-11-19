var git = require("../");
var normalizeOid = require("./util/normalize_oid");

var Odb = git.Odb;
var read = Odb.prototype.read;

Odb.prototype.read = function(oid, callback) {
  oid = normalizeOid(oid);

  return read.call(this, oid).then(function(odbObject) {
    if (typeof callback === "function") {
      callback(null, odbObject);
    }

    return odbObject;
  }, callback);
};

module.exports = Odb;
