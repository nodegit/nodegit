var git = require('../'),
    success = require('./utilities').success;

/**
 * Convenience patch class.
 *
 * @constructor
 */
var Patch = function(rawPatch) {
  this.rawPatch = rawPatch;
};
