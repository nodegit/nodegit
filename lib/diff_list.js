var git = require('../'),
    events = require('events');

/**
 * Convenience diff list class.
 *
 * @constructor
 * @param {git.raw.DiffList} [rawDiffList = new git.raw.DiffList]  
 */
var DiffList = function(rawDiffList) {
  if (!(rawDiffList instanceof git.raw.DiffList)) {
    throw new Error('Parameter for DiffList must be a raw difflist');
  }
  this.rawDiffList = rawDiffList;
};

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
 * Retrieve patches
 *
 * @return {Array} patches
 */
DiffList.prototype.patches = function() {
  var size = this.rawDiffList.size();
  result = [];
  for (var i = 0; i < size; i++) {
    result.push(this.rawDiffList.patch(i));
  }
  return result;
};

exports.diffList = DiffList;

/**
 * @namespace
 * @property {Object} oldFile Contains details for the old file state 
 * @property {String} oldFile.path The path to the old file, relative to the repository 
 * @property {Object} newFile Contains details for the new file state
 * @property {String} newFile.path The path to the new file, relative to the repository
 * @property {Object[]} content Array of context & differences 
 * @property {Object} content[].range 
 * @property {Object} content[].range.old
 * @property {Integer} content[].range.old.start
 * @property {Integer} content[].range.old.lines
 * @property {Object} content[].range.new
 * @property {Integer} content[].range.new.start
 * @property {Integer} content[].range.new.lines
 * @property {Object} content[].content Content of the delta
 * @property {DiffList.lineOriginTypes} content[].lineOrigin
 * @property {Integer} content[].contentLength
 * @property {Integer} status Type of delta 
 */
var FileDelta = {
  oldFile: {
    path: String
  },
  newFile: {
    path: String
  },
  content: [
    {
      range: { 
        old: { 
          start: Number, 
          lines: Number 
        }, 
        'new': { 
          start: Number, 
          lines: Number 
        } 
      },
      content: String,
      lineOrigin: String,
      contentLength: Number
    }
  ],
  status: Number
};
