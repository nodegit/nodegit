var git = require('../'),
    DiffList = git.DiffList,
    ConvenientPatch = require('./convenient_patch').ConvenientPatch,
    events = require('events');

/**
 * Refer to vendor/libgit2/include/git2/diff.h for delta type definitions.
 *
 * @readonly
 * @enum {Integer}
 */
DiffList.Delta = {
  /** 0 */ Unmodified: 0,
  /** 1 */ Added: 1,
  /** 2 */ Deleted: 2,
  /** 3 */ Modified: 3,
  /** 4 */ Renamed: 4,
  /** 5 */ Copied: 5,
  /** 6 */ Ignored: 6,
  /** 7 */ Untracked: 7,
  /** 8 */ TypeChange: 8
};

/**
 * Refer to vendor/libgit2/include/git2/diff.h for line origin type definitions.
 *
 * @readOnly
 * @enum {String}
 */
DiffList.LineOrigin = {
  /** ' '  */ Context: 32,
  /** '+'  */ Addition: 43,
  /** '-'  */ Deletion: 45,
  /** '\n' */ AddEofNl: 13,
  /** ''   */ DelEofNl: 0,
  /** 'F'  */ FileHdr: 106,
  /** 'H'  */ HunkHdr: 110,
  /** 'B'  */ Binary: 102
};

/**
 * Retrieve patches in this difflist
 *
 * @return {[ConvenientPatch]} an array of ConvenientPatches
 */
DiffList.prototype.patches = function() {
  var size = this.size();
  result = [];
  for (var i = 0; i < size; i++) {
    result.push(new ConvenientPatch(this.patch(i)));
  }
  return result;
};
