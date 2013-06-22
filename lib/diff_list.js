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
    throw new git.error('Parameter for DiffList must be a raw difflist');
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
 /** ' '  */ Context: ' ',
 /** '+'  */ Addition: '+',
 /** '-'  */ Deletion: '-',
 /** '\n' */ AddEofNl: '\n',
 /** ''   */ DelEofNl: '',
 /** 'F'  */ FileHdr: 'F',
 /** 'H'  */ HunkHdr: 'H',
 /** 'B'  */ Binary: 'B'
};

DiffList.treeToTree = function(repo, oldSha, newSha, callback) {
  git.raw.DiffList.treeToTree(repo.rawRepo, oldSha, newSha, function(error, rawDiffList) {
    if (error) return callback(new git.error(error.message, error.code), null);

    callback(null, new DiffList(rawDiffList));
  });
};


/**
 * Walk the current diff list tree.
 *
 * @fires DiffList#delta
 * @fires DiffList#end
 * 
 * @return {EventEmitter} diffListWalkEmitter
 */
DiffList.prototype.walk = function() {
  var event = new events.EventEmitter(),
      allFileDeltas = [],
      self = this;

  self.rawDiffList.walk(function fileCallback(error, fileDeltas) {
    if (error) {
      event.emit('end', new git.error(error.message, error.code), null);
    }
    fileDeltas.forEach(function(fileDelta) {
      /**
       * Delta event.
       *
       * @event DiffList#delta
       *
       * @param {GitError|null} error An error object if there was an issue, null otherwise.
       * @param {FileDelta} fileDelta The file delta object.
       */
      event.emit('delta', null, fileDelta);
      allFileDeltas.push(fileDelta);
    });
  }, function hunkCallback(error, diffHunk) {
    /** TO BE IMPLEMENTED */
  }, function lineCallback(error, diffLine) {
    /** TO BE IMPLEMENTED */
  }, function endCallback(error) {
    /**
     * End event.
     *
     * @event DiffList#end
     *
     * @param {GitError|null} error An error object if there was an issue, null otherwise.
     * @param {FileDelta[]} fileDeltas The file delta objects.
     */
    event.emit('end', error ? new git.error(error.message, error.code) : null, allFileDeltas);
  });

  return event;
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
