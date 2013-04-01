var git = require('../');

var Signature = function(rawSignature) {
  if (rawSignature instanceof git.raw.Signature) {
    this.rawSignature = rawSignature;
  } else {
    this.rawSignature = new git.raw.Signature();
  }
};

Signature.prototype.name = function(callback) {
  callback(null, this.rawSignature.name());
};

Signature.prototype.email = function(callback) {
  callback(null, this.rawSignature.email());
};

Signature.prototype.duplicate = function(callback) {
  callback(null, new Signature(git.rawSignature.duplicate()));
};

exports.signature = Signature;
