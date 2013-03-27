var git = require('../'),
  success = require('./utilities').success;

var DIRECTORY = 33188;
var FILE = 16384;

var TreeEntry = function(rawRepo, rawTreeEntry) {
  if(!(rawRepo instanceof git.raw.Repo)) {
    throw git.error('First parameter for Tree Entry must be a raw repo', 0);
  }

  if(!(rawTreeEntry instanceof git.raw.TreeEntry)) {
    throw git.error('Second parameter for Tree Entry must be a raw tree entry', 0);
  }

  this.rawRepo = rawRepo;
  this.rawEntry = rawTreeEntry;
};

/**
 * Retrieve the Oid for this TreeEntry.
 *
 * @param  {Function} callback
 */
TreeEntry.prototype.oid = function(callback) {
  this.rawEntry.oid(function(error, rawOid) {
    if (success(error, callback)) {
      callback(null, git.oid(rawOid));
    }
  });
};

/**
 * Retrieve the SHA for this TreeEntry.
 *
 * @param  {Function} callback
 */
TreeEntry.prototype.sha = function(callback) {
  this.rawEntry.sha(function(error, sha) {
    if (success(error, callback)) {
      callback(null, sha);
    }
  });
};

/**
 * Determine whether this TreeEntry is a file.
 *
 * @param  {Function} callback
 */
TreeEntry.prototype.isFile = function(callback) {
  var self = this;
  if (typeof self._cache.fileMode !== 'undefined') {
    callback(null, self._cache.fileMode === FILE);
    return;
  }
  self.rawEntry.fileMode(function(error, fileMode) {
    if (success(error, callback)) {
      self._cache.fileMode = fileMode;
      callback(null, self._cache.fileMode === FILE);
    }
  });
};

/**
 * Determine whether this Tree Entry is a directory.
 *
 * @param  {Function} callback
 */
TreeEntry.prototype.isDirectory = function(callback) {
  var self = this;
  if (typeof self._cache.fileMode !== 'undefined') {
    callback(null, self._cache.fileMode === DIRECTORY);
    return;
  }
  self.rawEntry.fileMode(function(error, fileMode) {
    if (success(error, callback)) {
      self._cache.fileMode = fileMode;
      callback(null, self._cache.fileMode === DIRECTORY);
    }
  });
};

/**
 * Retrieve the name for this TreeEntry.
 *
 * @param  {Function} callback
 */
TreeEntry.prototype.name = function(callback) {
  this.rawEntry.name(function(error, name) {
    if (success(error, callback)) {
      callback(null, name);
    }
  });
};

/**
 * Retrieve the TreeEntry's content.
 *
 * @param  {Function} callback
 */
TreeEntry.prototype.content = function(callback) {
  this.toBlob(function convertBlob(error, blob) {
    if (!success(error, callback)) {
      return;
    }
    blob.content(function blobContent(error, content) {
      if (success(error, callback)) {
        callback(null, content);
      }
    });
  });
};

/**
 * Convert the TreeEntry to a blob.
 *
 * @param  {Function} callback
 */
TreeEntry.prototype.toBlob = function(callback) {
  var self = this;
  self.rawEntry.toBlob(self.rawRepo, function blobCallback(error, rawBlob) {
    if (success(error, callback)) {
      callback(null, new git.blob(self.rawRepo, rawBlob));
    }
  });
};

/**
 * Retrieve the TreeEntry's Tree.
 *
 * @param  {Function} callback
 */
TreeEntry.prototype.tree = function(callback) {
  var self = this;
  self.oid(function treeEntryOid(error, oid) {
    if (!success(error, callback)) {
      return;
    }
    (new git.raw.Tree(self.rawRepo)).lookup(oid.getRawOid(), function(error, tree) {
      if (!success(error, callback)) {
        return;
      }
      callback(null, tree);
    });
  });
};

exports.entry = TreeEntry;
