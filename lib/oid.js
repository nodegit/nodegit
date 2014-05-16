var git = require('../'),
    Oid = git.Oid;

// Backwards compatibility.
Object.defineProperties(Oid.prototype, {
  "toString": {
    value: Oid.prototype.allocfmt,
    enumerable: false
  }
});

Object.defineProperties(Oid, {
  "fromString": {
    value: Oid.fromstr,
    enumerable: false
  }
});

Oid.prototype.inspect = function() {
  return "[Oid " + this.allocfmt() + "]";
};
