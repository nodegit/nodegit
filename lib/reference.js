var git = require('../'),
    success = require('./utilities').success;

/**
 * Convenience reference constructor.
 *
 * @constructor
 * @param {git.raw.Repo} rawRepo
 * @param {git.raw.Reference} [rawReference = new git.raw.Reference()]
 */
var Reference = function(rawRepo, rawReference) {
  if (!(rawRepo instanceof git.raw.Repo)) {
    throw new git.error('First parameter for Reference must be a raw repo');
  }
  this.rawRepo = rawRepo;

  if (rawReference instanceof git.raw.Reference) {
    this.rawReference = rawReference;
  } else {
    this.rawReference = new git.raw.Reference(this.rawRepo);
  }
};

/**
 * Lookup the reference with the given name.
 *
 * @param {String} name
 * @param {Reference~lookupCallback} callback
 */
Reference.prototype.lookup = function(name, callback) {
  /**
   * @callback Reference~lookupCallback Callback executed on lookup completion.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Reference|null} reference Retrieved reference object or null.
   */
  var self = this;
  self.rawReference.lookup(self.rawRepo, name, function referenceLookup(error, rawReference) {
    if (!success(error, callback)) {
      return;
    }
    self.rawReference = rawReference;
    callback(null, self);
  });
};

/**
 * Get the Oid representing this reference.
 *
 * @param {Reference~oidCallback} callback
 */
Reference.prototype.oid = function(callback) {
  /**
   * @callback Reference~oidCallback Callback executed on oid retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Oid|null} oid Retrieved Oid object or null.
   */
  this.rawReference.oid(function referenceOid(error, rawOid) {
    if (success(error, callback)) {
      callback(null, new git.oid(rawOid));
    }
  });
};

exports.reference = Reference;
