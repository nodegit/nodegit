var NodeGit = require("../");
var Promise = require("nodegit-promise");
var Diff = NodeGit.Diff;
var ConvenientHunk = NodeGit.ConvenientHunk;

function ConvenientPatch(delta, patch, i) {
  this.delta = delta;
  this.patch = patch;
  this.i = i;
}

/**
 * Old name of the file
 * @return {String}
 */
ConvenientPatch.prototype.oldFile = function() {
  return this.delta.oldFile();
};

/**
 * New name of the file
 * @return {String}
 */
ConvenientPatch.prototype.newFile = function() {
  return this.delta.newFile();
};

/**
 * The number of hunks in this patch
 * @return {Number}
 */
ConvenientPatch.prototype.size = function() {
  return this.patch.numHunks();
};

/**
 * The hunks in this patch
 * @async
 * @return {Array<ConvenientHunk>}  a promise that resolves to an array of
 *                                      ConvenientHunks
 */
ConvenientPatch.prototype.hunks = function() {
  var _this = this;
  var size = _this.size();
  var hunkPromises = [];
  var i;

  function makeHunkPromise(i) {
    return _this.patch.getHunk(i)
      .then(function(hunkWithLineCount) {
        return new ConvenientHunk(
          hunkWithLineCount.hunk,
          hunkWithLineCount.linesInHunk,
          _this.patch,
          i
        );
      });
  }

  for (i = 0; i < size; i++) {
    hunkPromises.push(makeHunkPromise(i));
  }

  return Promise.all(hunkPromises);
};

/**
 * The status of this patch (unmodified, added, deleted)
 * @return {Number}
 */
ConvenientPatch.prototype.status = function() {
  return this.delta.status();
};

/**
 * Is this an unmodified patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isUnmodified = function() {
  return this.status() == Diff.DELTA.UNMODIFIED;
};

/**
 * Is this an added patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isAdded = function() {
  return this.status() == Diff.DELTA.ADDED;
};

/**
 * Is this a deleted patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isDeleted = function() {
  return this.status() == Diff.DELTA.DELETED;
};

/**
 * Is this an modified patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isModified = function() {
  return this.status() == Diff.DELTA.MODIFIED;
};

/**
 * Is this a renamed patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isRenamed = function() {
  return this.status() == Diff.DELTA.RENAMED;
};

/**
 * Is this a copied patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isCopied = function() {
  return this.status() == Diff.DELTA.COPIED;
};

/**
 * Is this an ignored patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isIgnored = function() {
  return this.status() == Diff.DELTA.IGNORED;
};

/**
 * Is this an untracked patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isUntracked = function() {
  return this.status() == Diff.DELTA.UNTRACKED;
};

/**
 * Is this a type change?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isTypeChange = function() {
  return this.status() == Diff.DELTA.TYPECHANGE;
};

/**
 * Is this an undreadable patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isUnreadable = function() {
  return this.status() == Diff.DELTA.UNREADABLE;
};

/**
 * Is this a conflicted patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isConflicted = function() {
  return this.status() == Diff.DELTA.CONFLICTED;
};

NodeGit.ConvenientPatch = ConvenientPatch;
