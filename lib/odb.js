var git = require('../'),
    util = require('./util.js'),
    Odb = git.Odb;

/**
 * Retrieve the object identified by oid.
 *
 * @param {String|Oid} String sha or Oid
 * @param {Function} callback
 * @return {git.Object} a git odb object
 */
util.normalizeOid(Odb.prototype, 'read');
util.makeSafe(Odb.prototype, 'read');
