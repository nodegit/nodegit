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
  /** 0 */  GITERR_NOMEMORY: git.raw.Error.codesGITERR_NOMEMORY,
  /** 1 */  GITERR_OS: git.raw.Error.codesGITERR_OS,
  /** 2 */  GITERR_INVALID: git.raw.Error.codesGITERR_INVALID,
  /** 3 */  GITERR_REFERENCE: git.raw.Error.codesGITERR_REFERENCE,
  /** 4 */  GITERR_ZLIB: git.raw.Error.codesGITERR_ZLIB,
  /** 5 */  GITERR_REPOSITORY: git.raw.Error.codesGITERR_REPOSITORY,
  /** 6 */  GITERR_CONFIG: git.raw.Error.codesGITERR_CONFIG,
  /** 7 */  GITERR_REGEX: git.raw.Error.codesGITERR_REGEX,
  /** 8 */  GITERR_ODB: git.raw.Error.codesGITERR_ODB,
  /** 9 */  GITERR_INDEX: git.raw.Error.codesGITERR_INDEX,
  /** 10 */ GITERR_OBJECT: git.raw.Error.codesGITERR_OBJECT,
  /** 11 */ GITERR_NET: git.raw.Error.codesGITERR_NET,
  /** 12 */ GITERR_TAG: git.raw.Error.codesGITERR_TAG,
  /** 13 */ GITERR_TREE: git.raw.Error.codesGITERR_TREE
};

/**
 * Refer to vendor/libgit2/include/git2/errors.h for return code definitions.
 *
 * @readonly
 * @enum {Integer}
 */
GitError.prototype.returnCodes = {
  /** 0   */ GIT_OK: git.raw.Error.returnCodesGIT_OK,
  /** -1  */ GIT_ERROR: git.raw.Error.returnCodesGIT_ERROR,
  /** -3  */ GIT_ENOTFOUND: git.raw.Error.returnCodesGIT_ENOTFOUND,
  /** -4  */ GIT_EEXISTS: git.raw.Error.returnCodesGIT_EEXISTS,
  /** -5  */ GIT_EAMBIGUOUS: git.raw.Error.returnCodesGIT_EAMBIGUOUS,
  /** -6  */ GIT_EBUFS: git.raw.Error.returnCodesGIT_EBUFS,
  /** -30 */ GIT_PASSTHROUGH: git.raw.Error.returnCodesGIT_PASSTHROUGH,
  /** -31 */ GIT_ITEROVER: git.raw.Error.returnCodesGIT_ITEROVER
};

exports.error = function(error) {
    return new GitError(error.message, error.code);
};
