var git = require( '../' );

var _Util = function( error ) {
  var self = {};

  self.error = function error( err ) {
    if(err !== 0) {
      return git.error( err );
    }

    return 0;
  };

  self.asyncComplete = function(args, callback) {
    args = Array.prototype.slice.call(args);
    args[0] = git.util().error(args[0]);

    callback.apply(this, args.concat(this));
  };

  return self;
};

exports.util = _Util;
