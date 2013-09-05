var git = require('../'),
    Oid = git.Oid;

/**
 * The hex representation of the SHA
 * @return {String}
 */
Oid.prototype.toString = function() {
  return this.sha();
};

Oid.prototype.inspect = function() {
  return "[Oid " + this.sha() + "]";
};
