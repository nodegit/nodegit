var NodeGit = require("../");

var Odb = NodeGit.Odb;
var read = Odb.prototype.read;

Odb.prototype.read = function(oid, callback) {
  return read.call(this, oid).then(function(odbObject) {
    if (typeof callback === "function") {
      callback(null, odbObject);
    }

    return odbObject;
  }, callback);
};
