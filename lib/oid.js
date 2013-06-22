var git = require('../'),
    success = require('./utilities').success;

/**
 * Convenience Oid constructor.
 *
 * @constructor
 * @param {git.raw.Oid} [rawOid = new git.rawOid] Raw Oid object.
 */
var Oid = function(rawOid) {
  this.rawOid = rawOid;
};

/**
 * Create Oid object from string.
 *
 * @param  {String} sha
 */
Oid.fromString = function(sha) {
  var rawOid = git.raw.Oid.fromString(sha);
  return new Oid(rawOid);
};

/**
 * @return {git.raw.Oid} The wrapped raw Oid object.
 */
Oid.prototype.getRawOid = function() {
  return this.rawOid;
};

/**
 * Convert the raw Oid to a SHA
 */
Oid.prototype.sha = function() {
  return this.rawOid.sha();
};

exports.oid = Oid;
