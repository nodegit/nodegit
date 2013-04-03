var git = require('../'),
    success = require('./utilities').success;

/**
 * Convenience Oid constructor.
 *
 * @constructor
 * @param {git.raw.Oid} [rawOid = new git.rawOid] Raw Oid object.
 */
var Oid = function(rawOid) {
  if(rawOid instanceof git.raw.Oid) {
    this.rawOid = rawOid;
  } else {
    this.rawOid = new git.raw.Oid();
  }
};

/**
 * @return {git.raw.Oid} The wrapped raw Oid object.
 */
Oid.prototype.getRawOid = function() {
  return this.rawOid;
};

/**
 * Create Oid object from string.
 *
 * @param  {String} sha
 * @param  {Oid~fromStringCallback} callback
 */
Oid.prototype.fromString = function(sha, callback) {
  /**
   * @callback Oid~fromStringCallback Callback executed after raw Oid is created.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Oid|null} oid The new Oid object.
   */
  var self = this;
  self.rawOid.fromString(sha, function(error, rawOid) {
    if (success(error, callback)) {
      self.rawOid = rawOid;
      callback(null, self);
    }
  });
};

/**
 * Convert the raw Oid to a SHA
 *
 * @param  {Oid~shaCallback} callback
 */
Oid.prototype.sha = function(callback) {
  /**
   * @callback Oid~shaCallback Callback executed after SHA is retrieved.
   * @param {GitError|null} error An Error or null if successful.
   * @param {String|null} sha The SHA.
   */
  callback(null, this.rawOid.sha());
};

exports.oid = Oid;
