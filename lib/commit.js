var git = require( '../' );

var _Commit = function( repo ) {
  var self = {};

  // Internal reference to a Git reference
  self.commit = commit || new git.git2.Commit( repo );

  return self;
};

exports.commit = _Commit;
