var NodeGit = require("../");
var TreeEntry = require("./tree_entry");
var LookupWrapper = require("./util/lookupWrapper");

var Blob = NodeGit.Blob;

/**
* Retrieves the blob pointed to by the oid
* @param {Repository} repo The repo that the blob lives in
* @param {String|Oid|Blob} id The blob to lookup
* @param {Function} callback
* @return {Blob}
*/
Blob.lookup = LookupWrapper(Blob);

/**
 * Retrieve the content of the Blob.
 *
 * @return {Buffer} Contents.
 */
Blob.prototype.content = function() {
  return this.rawcontent().toBuffer(this.rawsize());
};

/**
 * Retrieve the Blob's content as String.
 *
 * @return {string} Contents.
 */
Blob.prototype.toString = function() {
  return this.content().toString();
};

/**
 * Retrieve the Blob's type.
 *
 * @return {number} The filemode.
 */
Blob.prototype.filemode = function() {
  var FileMode = TreeEntry.FILEMODE;

  return this.isBinary() ? FileMode.EXECUTABLE : FileMode.BLOB;
};

module.exports = Blob;
