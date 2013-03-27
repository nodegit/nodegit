var git = require('../'),
  success = require('./utilities').success;

/**
 * Blob convenience class constructor.
 *
 * @param {git.raw.Repo} rawRepo
 * @param {git.raw.Blob|null} rawBlob
 */
var Blob = function(rawRepo, rawBlob) {
  if(!(rawRepo instanceof git.raw.Repo)) {
    throw git.error('First parameter for Blob must be a raw repo');
  }
  this.rawRepo = rawRepo;

  if(rawBlob instanceof git.raw.Blob) {
    this.rawBlob = rawBlob;
  } else {
    this.rawBlob = new git.raw.Blob(this.rawRepo);
  }
};

/**
 * Retrieve the blob represented by the oid.
 *
 * @param  {git.raw.Oid} oid
 * @param  {Function} callback
 */
Blob.prototype.lookup = function(oid, callback) {
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
 * @param  {Function} callback
 */
Blob.prototype.rawContent = function(callback) {
  this.rawBlob.rawContent(function(error, content) {
    if (success(error, callback)) {
      callback(null, content);
    }
  });
};

/**
 * Retrieve the blob's content.
 *
 * @param  {Function} callback
 */
Blob.prototype.content = function(callback) {
  this.rawContent(function(error, content) {
    if (success(error, callback)) {
      callback(null, content.toString());
    }
  });
};

exports.blob = Blob;
