var git = require( '../' );

var Oid = function(rawOid) {
  var self = {};

  if(rawOid instanceof git.raw.Oid) {
    self.oid = rawOid;
  } else {
    self.oid = new git.raw.Oid();
  }

  self.getRawOid = function() {
    return self.oid;
  };

  return self;
};

exports.oid = Oid;
