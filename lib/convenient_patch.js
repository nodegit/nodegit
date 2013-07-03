var git = require('../'),
    DiffList = git.DiffList,
    ConvenientHunk = require('./convenient_hunk').ConvenientHunk;

function ConvenientPatch(raw) {
  this.raw = raw;
}

ConvenientPatch.prototype.oldFile = function() {
  return this.raw.delta.oldFile();
};

ConvenientPatch.prototype.newFile = function() {
  return this.raw.delta.newFile();
};

ConvenientPatch.prototype.size = function() {
  return this.raw.patch.size();
};

ConvenientPatch.prototype.status = function() {
  return this.raw.delta.status();
};

ConvenientPatch.prototype.isUnmodified = function() {
  return this.status() == DiffList.Delta.Unmodified;
};

ConvenientPatch.prototype.isAdded = function() {
  return this.status() == DiffList.Delta.Added;
};

ConvenientPatch.prototype.isDeleted = function() {
  return this.status() == DiffList.Delta.Deleted;
};

ConvenientPatch.prototype.isModified = function() {
  return this.status() == DiffList.Delta.Modified;
};

ConvenientPatch.prototype.isRenamed = function() {
  return this.status() == DiffList.Delta.Renamed;
};

ConvenientPatch.prototype.isCopied = function() {
  return this.status() == DiffList.Delta.Copied;
};

ConvenientPatch.prototype.isIgnored = function() {
  return this.status() == DiffList.Delta.Ignored;
};

ConvenientPatch.prototype.isUntracked = function() {
  return this.status() == DiffList.Delta.Untracked;
};

ConvenientPatch.prototype.isTypeChange = function() {
  return this.status() == DiffList.Delta.TypeChange;
};

ConvenientPatch.prototype.hunks = function() {
  var result = [];
  for (var i = 0; i < this.size(); i++)
    result.push(new ConvenientHunk(this.raw.patch, i));
  return result;
};

exports.ConvenientPatch = ConvenientPatch;
