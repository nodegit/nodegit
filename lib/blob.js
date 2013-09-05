var git = require('../'),
    TreeEntry = git.TreeEntry,
    Blob = git.Blob;

var oldContent = Blob.prototype.content;

/**
 * Retrieve the content of the blob.
 * @return {Buffer} content
 */
Blob.prototype.content = function() {
  return oldContent.call(this).toBuffer(this.size());
};

/**
 * Retrieve the blob's content as String.
 */
Blob.prototype.toString = function() {
  return this.content().toString();
};

/**
 * Retrieve the blob's type.
 */
Blob.prototype.filemode = function() {
  return this.isBinary() ? TreeEntry.FileMode.Executable : TreeEntry.FileMode.Blob;
};
