var git = require('../');

/**
 * Check if error is null, if it is not, convert it to a GitError and call
 * the callback.
 *
 * @param  {Object}   error
 * @param  {Function} callback
 * @return {Boolean} True if the error was null, false otherwise.
 */
exports.success = function(error, callback) {
  if (error) {
    if (error instanceof git.error) {
      callback(error);
    } else {
      callback(git.error(error));
    }
    return false;
  }
  return true;
};
