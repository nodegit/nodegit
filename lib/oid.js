var NodeGit = require("../");

var Oid = NodeGit.Oid;

// Backwards compatibility.
Object.defineProperties(Oid.prototype, {
  "toString": {
    value: Oid.prototype.tostrS,
    enumerable: false
  },
  "allocfmt": {
    value: Oid.prototype.tostrS,
    enumerable: false
  }
});

Oid.prototype.inspect = function() {
  return "[Oid " + this.allocfmt() + "]";
};
