var git = require('../'),
  success = require('./utilities').success;

/**
 * Blob convenience class constructor.
 *
 * @constructor
 * @param {git.raw.Repo} rawRepo Raw repository object.
 * @param {git.raw.Blob} [rawBlob = new git.raw.Blob(rawRepo)] Raw blob object.
 */
var Blob = function(rawRepo, rawBlob) {
  if(!(rawRepo instanceof git.raw.Repo)) {
    throw new git.error('First parameter for Blob must be a raw repo');
  }
  this.rawRepo = rawRepo;

  if(typeof rawBlob !== 'undefined' &&
      rawBlob instanceof git.raw.Blob) {
    this.rawBlob = rawBlob;
  } else {
    this.rawBlob = new git.raw.Blob(this.rawRepo);
  }
};

/**
 * Retrieve the blob represented by the oid.
 *
 * @param  {git.raw.Oid} oid The OID representing the blob to lookup.
 * @param {Blob~lookupCallback} callback
 */
Blob.prototype.lookup = function(oid, callback) {
  /**
   * @callback Blob~lookupCallback Callback executed on lookup completion.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Blob|null} blob Retrieved blob object or null.
   */
  var self = this;
  self.rawBlob.lookup(self.rawRepo, oid, function blobLookup(error, rawBlob) {
    if (success(error, callback)) {
      self.rawBlob = rawBlob;
      callback(null, self);
    }
  });
};

/**
 * Retrieve the blob's raw content buffer.
 *
 * @param {Blob~rawContentCallback} callback
 */
Blob.prototype.rawContent = function(callback) {
  /**
   * @callback Blob~rawContentCallback Callback executed after raw content is retrieved.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Buffer|null} content The raw content of the blob or null.
   */
  this.rawBlob.rawContent(function(error, content) {
    if (success(error, callback)) {
      callback(null, content);
    }
  });
};

/**
 * Retrieve the blob's content.
 *
 * @param {Blob~contentCallback} callback
 */
Blob.prototype.content = function(callback) {
  /**
   * @callback Blob~contentCallback Callback executed after content is retrieved.
   * @param {GitError|null} error An Error or null if successful.
   * @param {String|null} content The content of the blob or null.
   */
  this.rawContent(function(error, content) {
    if (success(error, callback)) {
      callback(null, content.toString());
    }
  });
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
  var self = this;
  self.rawBlob.createFromFile(path, self.rawRepo, function blobCreateFromFileCallback(error, rawBlob) {
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
