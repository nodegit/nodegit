var git = require('../'),
    Reference = git.Reference;

Reference.Type = {
  Oid: 1,
  Symbolic: 2
};

Reference.prototype.isOid = function() {
  return this.type() == Reference.Type.Oid;
};

Reference.prototype.isSymbolic = function() {
  return this.type() == Reference.Type.Symbolic;
};
