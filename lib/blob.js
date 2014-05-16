"use strict";

var git = require('../');
var TreeEntry = git.TreeEntry;
var Blob = git.Blob;

// Assign deprecated methods.
Blob.prototype.size = this.rawsize;

/**
 * Retrieve the content of the blob.
 * @return {Buffer} content
 */
Blob.prototype.content = function() {
  return this.rawcontent().toBuffer(this.rawsize());
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
