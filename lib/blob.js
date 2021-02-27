var util = require("util");
var NodeGit = require("../");
var Blob = NodeGit.Blob;
var LookupWrapper = NodeGit.Utils.lookupWrapper;
var TreeEntry = NodeGit.TreeEntry;

var _filteredContent = Blob.filteredContent;

/**
 * Retrieves the blob pointed to by the oid.
 *
 * @param {Repository}          repo  The repo that the blob lives in.
 * @param {string | Oid | Blob} id    The blob to lookup.
 * @returns {Blob}
 * @async
 */
Blob.lookup = LookupWrapper(Blob);

/**
 * Retrieve the content of the Blob.
 *
 * @returns {Buffer} Contents. As a buffer.
 */
Blob.prototype.content = function () {
  return this.rawcontent().toBuffer(this.rawsize());
};

/**
 * Retrieve the Blob's type.
 *
 * @returns {number} The. Filemode of the blob.
 */
Blob.prototype.filemode = function () {
  var FileMode = TreeEntry.FILEMODE;

  return this.isBinary() ? FileMode.EXECUTABLE : FileMode.BLOB;
};

/**
 * Retrieve the Blob's content as String.
 *
 * @returns {string} Contents. As a string.
 */
Blob.prototype.toString = function () {
  return this.content().toString();
};

Blob.filteredContent = util.deprecate(
  _filteredContent,
  "NodeGit.Blob.filteredContent is deprecated" + " use NodeGit.Blob.prototype.filter instead."
);
