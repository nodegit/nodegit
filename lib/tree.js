var git = require( '../' );

var _Tree = function( obj, tree ) {
  var self = {};

  //Object.defineProperty( self, 'length', {
  //  get: function() {
  //    return self.tree.entryCount();
  //  },
  //  enumerable: true
  //});

  self.each = function( callback ) {
    if( !callback ) { return; }

    var commit, i;
    for(i=0, len=self.length; i<len; i++) {
      commit = git.commit( self.repo );
      git.commit( self.tree.entryByIndex( commit.commit, i ) );
      callback.apply( commit, [ i, commit ] );
    }
  };

  // Internal references to Git references
  if( obj instanceof git.raw.Repo ) {
    // TODO: Add support for creation
    self.repo = obj;
    self.tree = tree;
  }
  else if ( obj instanceof git.raw.Tree ) {
    self.tree = obj;
  }
  else {
    self.tree = new git.raw.Tree();
  }

  return self;
};

exports.tree = _Tree;
