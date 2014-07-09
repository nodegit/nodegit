var NodeGit = require("../");
var TreeEntry = require("./tree_entry");
var Blob = NodeGit.Blob;

var FileMode = TreeEntry.FileMode;

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
  return this.isBinary() ? FileMode.Executable : FileMode.Blob;
};
