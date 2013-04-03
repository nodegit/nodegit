var git = require('../'),
    success = require('./utilities').success;

/**
 * Convenience reference constructor.
 *
 * @constructor
 * @param {git.raw.Repo} rawRepo
 * @param {git.raw.Reference|null} rawReference
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
 * @param  {String}   name
 * @param  {Function} callback
 */
Reference.prototype.lookup = function(name, callback) {
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
 * @param  {Function} callback
 */
Reference.prototype.oid = function(callback) {
  this.rawReference.oid(function referenceOid(error, rawOid) {
    if (success(error, callback)) {
      callback(null, new git.oid(rawOid));
    }
  });
};

exports.reference = Reference;
