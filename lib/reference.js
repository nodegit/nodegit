var NodeGit = require("../");
var LookupWrapper = require("./util/lookupWrapper");

var Reference = NodeGit.Reference;
var Branch = NodeGit.Branch;

/**
* Retrieves the reference pointed to by the oid
* @param {Repository} repo The repo that the reference lives in
* @param {String|Reference} id The reference to lookup
* @param {Function} callback
* @return {Reference}
*/
Reference.lookup = LookupWrapper(Reference);

/**
* Retrieves the reference by it's short name
* @param {Repository} repo The repo that the reference lives in
* @param {String|Reference} id The reference to lookup
* @param {Function} callback
* @return {Reference}
*/
Reference.dwim = LookupWrapper(Reference, Reference.dwim);

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
