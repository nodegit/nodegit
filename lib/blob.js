var git = require('../'),
  success = require('./utilities').success;

/**
 * Blob convenience class constructor.
 *
 * @constructor
 * @param {git.raw.Blob} [rawBlob = new git.raw.Blob(rawRepo)] Raw blob object.
 */
var Blob = function(rawBlob) {
  if (!(rawBlob instanceof git.raw.Blob)) {
    throw new Exception('First parameter for Blob must be a raw blob');
  }
  this.rawBlob = rawBlob;
};

/**
 * Retrieve the blob's raw content buffer.
 */
Blob.prototype.rawContent = function() {
  return this.rawBlob.content().toBuffer(this.size());
};

/**
 * Retrieve the blob's length.
 */
Blob.prototype.size = function() {
  return this.rawBlob.size();
};

/**
 * Retrieve the blob's content.
 */
Blob.prototype.content = function(callback) {
  var content = this.rawContent();
  return content.toString();
};

/**
 * Create a new blob from the file at the given path.
 *
 * @param  {String} path Full path to the file.
 * @param  {Blob~createFromFileCallback} callback
 */
Blob.prototype.createFromFile = function(path, callback) {
  /**
   * @callback Blob~createFromFileCallback Callback executed after blob is created.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Blob|null} blob The new blob or null.
   */
  git.raw.Blob.createFromFile(path, self.rawRepo, function blobCreateFromFileCallback(error, rawBlob) {
    if (success(error, callback)) {
      self.rawBlob = rawBlob;
      callback(null, self);
    }
  });
};

/**
 * Create a new blob from the given buffer.
 *
 * @param  {Buffer} buffer Buffer used to create blob.
 * @param  {Blob~createFromBufferCallback} callback
 */
Blob.prototype.createFromFile = function(path, callback) {
  /**
   * @callback Blob~createFromBufferCallback Callback executed after blob is created.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Blob|null} content The new blob or null.
   */
  var self = this;
  self.rawBlob.createFromBuffer(buffer, self.rawRepo, function blobCreateFromBufferCallback(error, rawBlob) {
    if (success(error, callback)) {
      self.rawBlob = rawBlob;
      callback(null, self);
    }
  });
};

exports.blob = Blob;
