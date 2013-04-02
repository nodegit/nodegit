var git = require('../'),
    util = require('util');

/**
 * GitError constructor.
 *
 * @constructor
 * @param {String} message The error description. Set from giterr_last->message.
 * @param {Integer} [code = git.raw.Error.codes.GITERR_INVALID] The error code. Set from giterr_last->klass
 */
var GitError = function(message, code) {
  Error.call(this);
  Error.captureStackTrace(this, exports.error);

  this.name = 'GitError';
  this.message = message;
  this.code = code || git.raw.Error.codes.GITERR_INVALID;
};

util.inherits(GitError, Error);

/**
 * Refer to vendor/libgit2/include/git2/errors.h for error code definitions.
 *
 * @readonly
 * @enum {Integer}
 */
GitError.prototype.codes = {
  /** 0 */  GITERR_NOMEMORY: git.raw.Error.codes['GITERR_NOMEMORY'],
  /** 1 */  GITERR_OS: git.raw.Error.codes['GITERR_OS'],
  /** 2 */  GITERR_INVALID: git.raw.Error.codes['GITERR_INVALID'],
  /** 3 */  GITERR_REFERENCE: git.raw.Error.codes['GITERR_REFERENCE'],
  /** 4 */  GITERR_ZLIB: git.raw.Error.codes['GITERR_ZLIB'],
  /** 5 */  GITERR_REPOSITORY: git.raw.Error.codes['GITERR_REPOSITORY'],
  /** 6 */  GITERR_CONFIG: git.raw.Error.codes['GITERR_CONFIG'],
  /** 7 */  GITERR_REGEX: git.raw.Error.codes['GITERR_REGEX'],
  /** 8 */  GITERR_ODB: git.raw.Error.codes['GITERR_ODB'],
  /** 9 */  GITERR_INDEX: git.raw.Error.codes['GITERR_INDEX'],
  /** 10 */ GITERR_OBJECT: git.raw.Error.codes['GITERR_OBJECT'],
  /** 11 */ GITERR_NET: git.raw.Error.codes['GITERR_NET'],
  /** 12 */ GITERR_TAG: git.raw.Error.codes['GITERR_TAG'],
  /** 13 */ GITERR_TREE: git.raw.Error.codes['GITERR_TREE']
};

/**
 * Refer to vendor/libgit2/include/git2/errors.h for return code definitions.
 *
 * @readonly
 * @enum {Integer}
 */
GitError.prototype.returnCodes = {
  /** 0   */ GIT_OK: git.raw.Error.returnCodes['GIT_OK'],
  /** -1  */ GIT_ERROR: git.raw.Error.returnCodes['GIT_ERROR'],
  /** -3  */ GIT_ENOTFOUND: git.raw.Error.returnCodes['GIT_ENOTFOUND'],
  /** -4  */ GIT_EEXISTS: git.raw.Error.returnCodes['GIT_EEXISTS'],
  /** -5  */ GIT_EAMBIGUOUS: git.raw.Error.returnCodes['GIT_EAMBIGUOUS'],
  /** -6  */ GIT_EBUFS: git.raw.Error.returnCodes['GIT_EBUFS'],
  /** -30 */ GIT_PASSTHROUGH: git.raw.Error.returnCodes['GIT_PASSTHROUGH'],
  /** -31 */ GIT_ITEROVER: git.raw.Error.returnCodes['GIT_ITEROVER']
}

exports.error = function(error) {
    return new GitError(error.message, error.code);
};
