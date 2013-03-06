var git = require('../'),
    util = require('util');

/**
 * Initialise an Error object
 *
 * @param  {mixed} obj A git.raw.Error object or a string describing the error.
 * @return {Object}
 */
var GitError = function(object) {
  var error = null;
  var gitError = new git.raw.Error();
  if (typeof object === 'number') {
    error = new Error(gitError.strError(object));
    error.code = object;
  }
  return error;
};

/**
 * Add libgit2 error codes to git.error object.
 *
 * Refer to vendor/libgit2/include/git2/errors.h for error code definitions.
 */
for (var errorName in git.raw.Error.codes) {
  GitError[errorName] = git.raw.Error.codes[errorName];
}

exports.error = GitError;
