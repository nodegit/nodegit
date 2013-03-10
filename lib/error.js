var git = require('../'),
    util = require('util');

/**
 * GitError constructor.
 *
 * @param {String} message giterr_last->message
 * @param {Integer} code giterr_last->klass
 */
var GitError = function(message, code) {
  Error.call(this);
  Error.captureStackTrace(this, exports.error);

  this.name = this.constructor.name;
  this.message = message;
  this.code = code;
};

util.inherits(GitError, Error);

/**
 * Add libgit2 error codes to git.error object.
 *
 * Refer to vendor/libgit2/include/git2/errors.h for error code definitions.
 */
for (var errorName in git.raw.Error.codes) {
  GitError.prototype[errorName] = git.raw.Error.codes[errorName];
}

exports.error = function(error) {
    return new GitError(error.message, error.code);
};
