var git = require('../'),
    DiffList = git.DiffList,
    ConvenientHunk = require('./convenient_hunk').ConvenientHunk;

function ConvenientPatch(raw) {
  this.raw = raw;
}

/**
 * Old name of the file
 * @return {String}
 */
ConvenientPatch.prototype.oldFile = function() {
  return this.raw.delta.oldFile();
};

/**
 * New name of the file
 * @return {String}
 */
ConvenientPatch.prototype.newFile = function() {
  return this.raw.delta.newFile();
};

/**
 * The number of hunks in this patch
 * @return {Number}
 */
ConvenientPatch.prototype.size = function() {
  return this.raw.patch.size();
};

/**
 * The hunks in this patch
 * @return {[ConvenientHunk]} an array of ConvenientHunks
 */
ConvenientPatch.prototype.hunks = function() {
  var result = [];
  for (var i = 0; i < this.size(); i++)
    result.push(new ConvenientHunk(this.raw.patch, i));
  return result;
};

/**
 * The status of this patch (unmodified, added, deleted)
 * @return {Number}
 */
ConvenientPatch.prototype.status = function() {
  return this.raw.delta.status();
};

/**
 * Is this an unmodified patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isUnmodified = function() {
  return this.status() == DiffList.Delta.Unmodified;
};

/**
 * Is this an added patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isAdded = function() {
  return this.status() == DiffList.Delta.Added;
};

/**
 * Is this a deleted patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isDeleted = function() {
  return this.status() == DiffList.Delta.Deleted;
};

/**
 * Is this an modified patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isModified = function() {
  return this.status() == DiffList.Delta.Modified;
};

/**
 * Is this a renamed patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isRenamed = function() {
  return this.status() == DiffList.Delta.Renamed;
};

/**
 * Is this a copied patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isCopied = function() {
  return this.status() == DiffList.Delta.Copied;
};

/**
 * Is this an ignored patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isIgnored = function() {
  return this.status() == DiffList.Delta.Ignored;
};

/**
 * Is this an untracked patch?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isUntracked = function() {
  return this.status() == DiffList.Delta.Untracked;
};

/**
 * Is this a type change?
 * @return {Boolean}
 */
ConvenientPatch.prototype.isTypeChange = function() {
  return this.status() == DiffList.Delta.TypeChange;
};

exports.ConvenientPatch = ConvenientPatch;
