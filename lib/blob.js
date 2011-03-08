var git = require( '../' );

var _Blob = function( obj ) {
  var self = {};

  if( obj instanceof git.raw.Repo ) {
    self.repo = obj;
    self.blob = new git.raw.Blob( obj );
  }
  else if ( obj instanceof git.raw.Blob ) {
    self.blob = obj;
  }

  self.content = function() {
    return self.blob.rawContent();
  };

  return self;
};

exports.blob = _Blob;
