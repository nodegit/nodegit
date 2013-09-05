var git = require('../'),
    Reference = git.Reference;

var oldSymbolicTarget = Reference.prototype.symbolicTarget,
    oldTarget = Reference.prototype.target;

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
 * Returns the target of this symbolic reference.
 * @return {Reference}
 * @throws if the target is not symbolic.
 */
Reference.prototype.symbolicTarget = function() {
  if (!this.isSymbolic()) throw this.name() + " is not symbolic";

  return oldSymbolicTarget.call(this);
};

/**
 * Returns the oid of this non-symbolic reference.
 * @return {Oid}
 * @throws if the target is symbolic.
 */
Reference.prototype.target = function() {
  if (!this.isConcrete()) throw this.name() + " is symbolic";

  return oldTarget.call(this);
};

/**
 * Returns the name of the reference.
 * @return {String}
 */
Reference.prototype.toString = function() {
 return this.name();
}