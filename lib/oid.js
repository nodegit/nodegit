var git = require( '../' );

/**
 * @param {git.raw.Oid|null} rawOid
 */
var Oid = function(rawOid) {
  if(rawOid instanceof git.raw.Oid) {
    this.rawOid = rawOid;
  }
};

/**
 * @return {git.raw.Oid}
 */
Oid.prototype.getRawOid = function() {
  return this.rawOid;
};

exports.oid = Oid;
