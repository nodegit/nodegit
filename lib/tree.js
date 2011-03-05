var git = require( '../' );

var _Tree = function( obj ) {
  var self = {};

  Object.defineProperty( self, 'length', {
    get: function() {
      return self.tree.entryCount();
    },
    enumerable: true
  });

  // Internal references to Git references
  if( obj instanceof git.git2.Repo ) {
    // TODO: Add support for creation
  }
  else if ( obj instanceof git.git2.Tree ) {
    self.tree = obj;
  }
  else {
    self.tree = new git.git2.Tree();
  }

  return self;
};

exports.tree = _Tree;
