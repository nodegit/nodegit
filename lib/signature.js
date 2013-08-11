var git = require('../'),
    Signature = git.Signature;

/**
 * Standard string representation of an author.
 */
Signature.prototype.toString = function() {
  return this.name().toString() + " <" + this.email().toString() + ">";
};
