var git = require("../../");

/**
 * Normalize an identifier to always be an OID instance.
 *
 * @param {String, Object} oid - The oid string or instance.
 * @return {Object} An Oid instance.
 */
function normalizeOid(oid) {
  try {
    return typeof oid === "string" ? git.Oid.fromString(oid) : oid;
  }
  catch (ex) {
    return null;
  }
}

module.exports = normalizeOid;
