var git = require('../');

/**
 * Check if error is null, if it is not, convert it to a GitError and call
 * the callback.
 *
 * @param  {Object} error
 * @param  {Function} callback
 *
 * @return {Boolean} True if the error was null, false otherwise.
 */
exports.success = function(error, callback) {
  if (error) {
    if (error instanceof git.error) {
      callback(error);
    } else {
      callback(new git.error(error.message, error.code));
    }
    return false;
  }
  return true;
};
