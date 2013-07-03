var git = require('../'),
    util = require('./util.js'),
    Odb = git.Odb;

/**
 * Retrieve the object identified by oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Repo~commitCallback} callback
 */
var oldRead = Odb.prototype.read;
Odb.prototype.read = function(oid, callback) {
  var self = this;
  oldRead.call(this, util.normalizeOid(oid), callback);
};
util.makeSafe(Odb.prototype, 'read');
