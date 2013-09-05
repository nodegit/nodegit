var git = require('../'),
    Signature = git.Signature;

/**
 * Standard string representation of an author.
 * @return {String}
 */
Signature.prototype.toString = function() {
  return this.name().toString() + " <" + this.email().toString() + ">";
};
