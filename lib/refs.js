var NodeGit = require("../");

var Reference = NodeGit.Refs;

Reference.Type = {
  Oid: 1,
  Symbolic: 2,
  All: 3
};

/**
 * Returns true if this reference is not symbolic
 * @return {Boolean}
 */
Reference.prototype.isConcrete = function() {
  return this.type() == Reference.Type.Oid;
};

/**
 * Returns true if this reference is symbolic
 * @return {Boolean}
 */
Reference.prototype.isSymbolic = function() {
  return this.type() == Reference.Type.Symbolic;
};

/**
 * Returns the name of the reference.
 * @return {String}
 */
Reference.prototype.toString = function() {
 return this.name();
};

module.exports = Reference;
