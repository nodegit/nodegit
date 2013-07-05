var git = require('../'),
    Oid = git.Oid;

Oid.prototype.toString = function() {
  return this.sha();
};

Oid.prototype.inspect = function() {
  return "[Oid " + this.sha() + "]";
};
