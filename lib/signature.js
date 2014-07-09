var NodeGit = require("../");
var Signature = NodeGit.Signature;

/**
 * Standard string representation of an author.
 *
 * @return {string} Representation of the author.
 */
Signature.prototype.toString = function() {
  return this.name().toString() + " <" + this.email().toString() + ">";
};

module.exports = Signature;
