var NodeGit = require("../");

var Odb = NodeGit.Odb;

var _read = Odb.prototype.read;

Odb.prototype.read = function(oid, callback) {
  return _read.call(this, oid).then(function(odbObject) {
    if (typeof callback === "function") {
      callback(null, odbObject);
    }

    return odbObject;
  }, callback);
};
