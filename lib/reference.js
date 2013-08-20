var git = require('../'),
    Reference = git.Reference;

Reference.Type = {
  Oid: 1,
  Symbolic: 2,
  All: 3
};

/**
 * Returns true if this reference is not symbolic
 */
Reference.prototype.isConcrete = function() {
  return this.type() == Reference.Type.Oid;
};

/**
 * Returns true if this reference is symbolic
 */
Reference.prototype.isSymbolic = function() {
  return this.type() == Reference.Type.Symbolic;
};

/**
 * Returns the target of this symbolic reference.
 * @throws if the target is not symbolic.
 */
var oldSymbolicTarget = Reference.prototype.symbolicTarget;
Reference.prototype.symbolicTarget = function() {
  if (!this.isSymbolic()) throw this.name() + " is not symbolic";

  return oldSymbolicTarget.call(this);
};

/**
 * Returns the oid of this non-symbolic reference.
 * @throws if the target is symbolic.
 */
var oldTarget = Reference.prototype.target;
Reference.prototype.target = function() {
  if (!this.isConcrete()) throw this.name() + " is symbolic";

  return oldTarget.call(this);
};

Reference.prototype.toString = function() {
 return this.name();
}