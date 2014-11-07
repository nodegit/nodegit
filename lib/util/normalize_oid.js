var NodeGit = require("../../");

/**
 * Normalize an identifier to always be an OID instance.
 *
 * @param {String, Object} oid - The oid string or instance.
 * @return {Object} An Oid instance.
 */
function normalizeOid(oid) {
  try {
    return typeof oid === "string" ? NodeGit.Oid.fromString(oid) : oid;
  }
  catch (ex) {
    console.log(ex);
    return null;
  }
}

module.exports = normalizeOid;
