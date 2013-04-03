var git = require('../'),
  success = require('./utilities').success;

var fileModeIsFile = function(fileMode, instance) {
  return fileMode === instance.fileModes.GIT_FILEMODE_BLOB ||
          fileMode === instance.fileModes.GIT_FILEMODE_BLOB_EXECUTABLE;
};

var fileModeIsDirectory = function(fileMode, instance) {
  return fileMode === instance.fileModes.GIT_FILEMODE_TREE;
};

/**
 * Convenience tree entry constructor.
 *
 * @constructor
 * @param {git.raw.Repo} rawRepo Raw repository object.
 * @param {git.raw.TreeEntry} rawTreeEntry Raw tree entry object.
 */
var TreeEntry = function(rawRepo, rawTreeEntry) {
  if(!(rawRepo instanceof git.raw.Repo)) {
    throw new git.error('First parameter for Tree Entry must be a raw repo', 0);
  }

  if(!(rawTreeEntry instanceof git.raw.TreeEntry)) {
    throw new git.error('Second parameter for Tree Entry must be a raw tree entry', 0);
  }

  this.rawRepo = rawRepo;
  this.rawEntry = rawTreeEntry;
  this._cache = {};
};

/**
 * Refer to vendor/libgit2/include/git2/types.h for filemode definitions.
 *
 * @readonly
 * @enum {Integer}
 */
TreeEntry.prototype.fileModes = {
  /** 0000000 */ GIT_FILEMODE_NEW: git.raw.TreeEntry.fileModes.GIT_FILEMODE_NEW,
  /** 0040000 */ GIT_FILEMODE_TREE: git.raw.TreeEntry.fileModes.GIT_FILEMODE_TREE,
  /** 0100644 */ GIT_FILEMODE_BLOB: git.raw.TreeEntry.fileModes.GIT_FILEMODE_BLOB,
  /** 0100755 */ GIT_FILEMODE_BLOB_EXECUTABLE: git.raw.TreeEntry.fileModes.GIT_FILEMODE_BLOB_EXECUTABLE,
  /** 0120000 */ GIT_FILEMODE_LINK: git.raw.TreeEntry.fileModes.GIT_FILEMODE_LINK,
  /** 0160000 */ GIT_FILEMODE_COMMIT: git.raw.TreeEntry.fileModes.GIT_FILEMODE_COMMIT
};

/**
 * Retrieve the Oid for this TreeEntry.
 *
 * @param {TreeEntry~oidCallback} callback
 */
TreeEntry.prototype.oid = function(callback) {
  /**
   * @callback TreeEntry~oidCallback Callback executed after the Oid is retrieved.
   * @param {GitError|null} error An Error or null if successful.
   * @param {TreeEntry|null} oid The Oid object or null.
   */
  this.rawEntry.oid(function(error, rawOid) {
    if (success(error, callback)) {
      callback(null, git.oid(rawOid));
    }
  });
};

/**
 * Retrieve the SHA for this TreeEntry.
 *
 * @param {TreeEntry~shaCallback} callback
 */
TreeEntry.prototype.sha = function(callback) {
  /**
   * @callback TreeEntry~shaCallback Callback executed after the SHA is retrieved.
   * @param {GitError|null} error An Error or null if successful.
   * @param {String|null} sha The SHA object or null.
   */
  this.rawEntry.oid(function(error, oid) {
    if (!success(error, callback)) {
      return;
    }
    (new git.oid(oid)).sha(function(error, sha) {
      if (success(error, callback)) {
        callback(null, sha);
      }
    });
  });
};

/**
 * Determine whether this TreeEntry is a file.
 *
 * @param {TreeEntry~isFileCallback} callback
 */
TreeEntry.prototype.isFile = function(callback) {
  /**
   * @callback TreeEntry~isFileCallback Callback executed after type is determined.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Boolean|null} content True if the entry is a file, false otherwise.
   */
  var self = this;
  if (typeof self._cache.fileMode !== 'undefined') {
    callback(null, fileModeIsFile(self._cache.fileMode, self));
    return;
  }
  self.rawEntry.fileMode(function(error, fileMode) {
    if (success(error, callback)) {
      self._cache.fileMode = fileMode;
      callback(null, fileModeIsFile(self._cache.fileMode, self));
    }
  });
};

/**
 * Determine whether this Tree Entry is a directory.
 *
 * @param {TreeEntry~isDirectoryCallback} callback
 */
TreeEntry.prototype.isDirectory = function(callback) {
  var self = this;
  if (typeof self._cache.fileMode !== 'undefined') {
    callback(null, fileModeIsDirectory(self._cache.fileMode, self));
    return;
  }
  self.rawEntry.fileMode(function(error, fileMode) {
    if (success(error, callback)) {
      self._cache.fileMode = fileMode;
      callback(null, fileModeIsDirectory(self._cache.fileMode, self));
    }
  });
};

/**
 * Retrieve the name for this TreeEntry.
 *
 * @param  {Function} callback
 */
TreeEntry.prototype.name = function(callback) {
  this.rawEntry.name(function treeEntryName(error, name) {
    if (success(error, callback)) {
      callback(null, name);
    }
  });
};

/**
 * Retrieve the entry's root path.
 *
 * @param  {Function} callback
 */
TreeEntry.prototype.root = function(callback) {
  this.rawEntry.root(function treeEntryRoot(error, root) {
    if (success(error, callback)) {
      callback(null, root);
    }
  });
};

/**
 * Retrieve the path relative to the repository root for this TreeEntry.
 *
 * @param  {Function} callback
 */
TreeEntry.prototype.path = function(callback) {
  var self = this;
  self.root(function treeEntryRoot(error, root) {
    if (!success(error, callback)) {
      return;
    }
    self.rawEntry.name(function treeEntryName(error, name) {
      if (success(error, callback)) {
        callback(null, root + name);
      }
    });
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
