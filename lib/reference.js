var NodeGit = require("../");

var Reference = NodeGit.Reference;
var Branch = NodeGit.Branch;

/**
 * Returns true if this reference is valid
 * @return {Boolean}
 */
Reference.prototype.isValid = function() {
  return this.type() != Reference.TYPE.INVALID;
};

/**
 * Returns true if this reference is not symbolic
 * @return {Boolean}
 */
Reference.prototype.isConcrete = function() {
  return this.type() == Reference.TYPE.OID;
};

/**
 * Returns true if this reference is symbolic
 * @return {Boolean}
 */
Reference.prototype.isSymbolic = function() {
  return this.type() == Reference.TYPE.SYMBOLIC;
};

/**
 * Returns the name of the reference.
 * @return {String}
 */
Reference.prototype.toString = function() {
 return this.name();
};

/**
 * Returns if the ref is pointed at by HEAD
 * @return {bool}
 */
Reference.prototype.isHead = function() {
  return Branch.isHead(this);
};

module.exports = Reference;
