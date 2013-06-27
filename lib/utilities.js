var git = require('../');

/**
 * @namespace
 */
var utilities = {
  /**
   * Check if error is null, if it is not, convert it to a GitError and call
   * the callback.
   *
   * @param  {Object} error
   * @param  {Function} callback
   *
   * @return {Boolean} True if the error was null, false otherwise.
   */
  success: function(error, callback) {
    if (typeof callback !== 'function') throw new Error('Callback must be provided');
    if (error) return callback(error);
    return true;
  }
};
exports.success = utilities.success;
