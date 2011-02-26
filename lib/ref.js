var git2 = require( '../' );

var _Ref = function( ref ) {
  var self = {};

  // Internal reference to a Git reference
  self.ref = ref || new git2.Ref();

  return self;
};

exports.ref = _Ref;
