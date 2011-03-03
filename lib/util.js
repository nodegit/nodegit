var git = require( '../' );

var _Util = function( error ) {
  var self = {};

  self.error = function error( err ) {
    if(err !== 0) {
      return git.error( err );
    }

    return 0;
  };

  return self;
};

exports.util = _Util;
