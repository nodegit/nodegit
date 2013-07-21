var git = require('../'),
    Reference = git.Reference;

Reference.Type = {
  Oid: 1,
  Symbolic: 2,
  All: 3
};

Reference.prototype.isOid = function() {
  return this.type() == Reference.Type.Oid;
};

Reference.prototype.isSymbolic = function() {
  return this.type() == Reference.Type.Symbolic;
};

var oldSymbolicTarget = Reference.prototype.symbolicTarget;
Reference.prototype.symbolicTarget = function() {
  if (!this.isSymbolic()) throw this.name() + " is not symbolic";

  return oldSymbolicTarget.call(this);
};

var oldOid = Reference.prototype.oid;
Reference.prototype.oid = function() {
  if (!this.isOid()) throw this.name() + " is not oid";

  return oldOid.call(this);
};
