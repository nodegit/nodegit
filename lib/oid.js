var NodeGit = require("../");

var Oid = NodeGit.Oid;

// Backwards compatibility.
Object.defineProperties(Oid.prototype, {
  "allocfmt": {
    value: Oid.prototype.tostrS,
    enumerable: false
  },
  "toString": {
    value: Oid.prototype.tostrS,
    enumerable: false
  }
});

Oid.prototype.copy = function() {
  return this.cpy(); // seriously???
};

Oid.prototype.inspect = function() {
  return "[Oid " + this.allocfmt() + "]";
};
