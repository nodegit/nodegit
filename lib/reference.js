var git = require('../');

/**
 * Convenience reference constructor.
 *
 * @constructor
 * @param {git.raw.Repo} rawRepo
 * @param {git.raw.Reference} [rawReference = new git.raw.Reference()]
 */
var Reference = function(rawReference) {
  if (!(rawReference instanceof git.raw.Reference)) {
    throw new Error('First parameter for Reference must be a raw reference');
  }
  this.rawReference = rawReference;
};

Reference.Type = {
  Oid: 0,
  Symbolic: 1
};

/**
 * Lookup the reference with the given name.
 *
 * @param {String} name
 * @param {Reference~lookupCallback} callback
 */
Reference.lookup = function(rawRepo, name, callback) {
  /**
   * @callback Reference~lookupCallback Callback executed on lookup completion.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Reference|null} reference Retrieved reference object or null.
   */
  var self = this;
  git.raw.Reference.lookup(rawRepo, name, function referenceLookup(error, rawReference) {
    if (rawReference.type() == Reference.Type.Symbolic) {
      rawReference.resolve(function referenceResolve(error, rawReference) {
        if (error) return callback(error);
        callback(null, new Reference(rawReference));
      });
    } else {
      if (error) return callback(error);
      callback(null, new Reference(rawReference));
    }
  });
};

/**
 * Get the Oid representing this reference.
 */
Reference.prototype.oid = function() {
  return this.rawReference.oid();
};

exports.reference = Reference;
