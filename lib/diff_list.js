var git = require('../'),
    events = require('events');

/**
 * Convenience diff list class.
 *
 * @constructor
 * @param {git.raw.Repo} rawRepo
 * @param {git.raw.DiffList} [rawDiffList = new git.raw.DiffList]  
 */
var DiffList = function(rawRepo, rawDiffList) {
  if (!(rawRepo instanceof git.raw.Repo)) {
    throw new git.error('First parameter for DiffList must be a raw repo');
  }
  this.rawRepo = rawRepo;

  if (rawDiffList instanceof git.raw.DiffList) {
    this.rawDiffList = rawDiffList;
  } else {
    this.rawDiffList = new git.raw.DiffList();
  }
};

/**
 * Refer to vendor/libgit2/include/git2/diff.h for delta type definitions.
 *
 * @readonly
 * @enum {Integer}
 */
DiffList.prototype.deltaTypes = {
  /** 0 */ GIT_DELTA_UNMODIFIED: git.raw.DiffList.deltaTypes.GIT_DELTA_UNMODIFIED,
  /** 1 */ GIT_DELTA_ADDED: git.raw.DiffList.deltaTypes.GIT_DELTA_ADDED,
  /** 2 */ GIT_DELTA_DELETED: git.raw.DiffList.deltaTypes.GIT_DELTA_DELETED,
  /** 3 */ GIT_DELTA_MODIFIED: git.raw.DiffList.deltaTypes.GIT_DELTA_MODIFIED,
  /** 4 */ GIT_DELTA_RENAMED: git.raw.DiffList.deltaTypes.GIT_DELTA_RENAMED,
  /** 5 */ GIT_DELTA_COPIED: git.raw.DiffList.deltaTypes.GIT_DELTA_COPIED,
  /** 6 */ GIT_DELTA_IGNORED: git.raw.DiffList.deltaTypes.GIT_DELTA_IGNORED,
  /** 7 */ GIT_DELTA_UNTRACKED: git.raw.DiffList.deltaTypes.GIT_DELTA_UNTRACKED,
  /** 8 */ GIT_DELTA_TYPECHANGE: git.raw.DiffList.deltaTypes.GIT_DELTA_TYPECHANGE 
};

/**
 * Refer to vendor/libgit2/include/git2/diff.h for line origin type definitions.
 *
 * @readOnly
 * @enum {String}
 */
DiffList.prototype.lineOriginTypes = {
 /** ' '  */ GIT_DIFF_LINE_CONTEXT: git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_CONTEXT,
 /** '+'  */ GIT_DIFF_LINE_ADDITION: git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_ADDITION,
 /** '-'  */ GIT_DIFF_LINE_DELETION: git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_DELETION,
 /** '\n' */ GIT_DIFF_LINE_ADD_EOFNL: git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_ADD_EOFNL,
 /** ''   */ GIT_DIFF_LINE_DEL_EOFNL: git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_DEL_EOFNL,
 /** 'F'  */ GIT_DIFF_LINE_FILE_HDR: git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_FILE_HDR,
 /** 'H'  */ GIT_DIFF_LINE_HUNK_HDR: git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_HUNK_HDR,
 /** 'B'  */ GIT_DIFF_LINE_BINARY: git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_BINARY
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

DiffList.prototype.treeToTree = function(oldSha, newSha, callback) {
  var self = this;
  self.rawDiffList.treeToTree(self.rawRepo, oldSha, newSha, function(error, rawDifflist) {
    if (error) {
      callback(new git.error(error.message, error.code), null);
      return;
    }
    self.rawDiffList = rawDifflist;
    callback(null, self);
  });  
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
