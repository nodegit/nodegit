var git2 = require('../build/default/git2');

var Ref = function( ref ) {
  var self = {};

  // Internal reference to a Git reference
  self.ref = ref || new git2.Ref();

  self.fmt = function() {
    return self.ref.fmt();
  };

  return self;
};

exports.ref = Ref;
