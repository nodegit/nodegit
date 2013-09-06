var git = require('../'),
    TreeBuilder = git.TreeBuilder,
    TreeEntry = git.TreeEntry,
    path = require('path');

var oldInsert = TreeBuilder.prototype.insert;

/**
 * Insert an object into this tree by oid
 *
 * @param {String} filename
 * @param {Oid} oid
 * @param {Number} filemode
 */
TreeBuilder.prototype.insert = function(filename, oid, filemode) {
  if (!this.insertions) this.insertions = [];

  this.insertions.push([filename, oid, filemode]);
};

/**
 * Insert a blob into this tree
 *
 * @param {String} filename
 * @param {Blob} blob
 * @param {Boolean} isExecutable
 */
TreeBuilder.prototype.insertBlob = function(filename, blob, isExecutable) {
  if (!this.blobs) this.blobs = [];

  this.blobs.push([filename, blob, isExecutable ? TreeEntry.FileMode.Executable : TreeEntry.FileMode.Blob]);
};

var oldWrite = TreeBuilder.prototype.write;

/**
 * Write this tree to the repo.
 *
 * @param {Function} callback
 */
TreeBuilder.prototype.write = function(callback) {
  var self = this;
  this.doInsertions(function(error) {
    if (error) return callback(error);

    if (self.builders && self.builders.length) {
      writeNextLevel(self.repo, self.builders, function(error, previousName, previousTreeId) {
        if (previousName && previousTreeId) {
          oldInsert.call(self, previousName, previousTreeId, TreeEntry.FileMode.Tree);
        }
        oldWrite.call(self, self.repo, callback);
      });
    } else {
      oldWrite.call(self, self.repo, callback);
    }
  })
};

TreeBuilder.prototype.doInsertions = function(callback) {
  var self = this;

  this.createBlobs(function(error) {
    if (error) return callback(error);

    self.doOidInsertions(callback);
  })
};

TreeBuilder.prototype.createBlobs = function(callback) {
  if (!this.blobs || !this.blobs.length) return callback();

  var self = this,
      data = this.blobs.pop(),
      path = data[0], buffer = data[1], filemode = data[2];

  this.repo.createBlobFromBuffer(buffer, function(error, blobId) {
    if (error) return callback(error);

    self.insert(path, blobId, filemode);
    self.createBlobs(callback);
  });
}

TreeBuilder.prototype.doOidInsertions = function(callback) {
  if (!this.insertions || !this.insertions.length) return callback();

  var self = this,
      data = this.insertions.pop(),
      filename = data[0], oid = data[1], filemode = data[2],
      parts = filename.split(path.sep),
      pathParts = parts.slice(0, parts.length - 1),
      filename = parts[parts.length - 1];

  insertOneLevel(this, pathParts, function(error, builder) {
    if (error) return callback(error);
    oldInsert.call(builder, filename, oid, filemode);
    self.doOidInsertions(callback);
  });
};

function writeNextLevel(repo, builders, callback, previousName, previousTreeId) {
  var builder = builders.pop();
  if (!builder) return callback(null, previousName, previousTreeId);

  if (previousName && previousTreeId) {
    oldInsert.call(builder, previousName, previousTreeId, TreeEntry.FileMode.Tree);
  }
  oldWrite.call(builder, repo, function(error, previousTreeId) {
    if (error) return callback(error);

    previousName = builder.name;
    writeNextLevel(repo, builders, callback, previousName, previousTreeId);
  });
}

function insertOneLevel(builder, parts, callback) {
  if (!parts.length) return callback(null, builder);

	var part = parts[0], rest = parts.slice(1, parts.length);
  if (!part) return insertOneLevel(builder, rest, callback);

  if (!builder.root.builders) builder.root.builders = [];
  var entry = builder.get(part);
  if (entry) {
    if (!entry.isTree()) return callback("Invalid path part " + part);
    entry.parent = builder;

    entry.getTree(function(error, tree) {
      if (error) return callback(error);

      var next = tree.builder();
      next.name = part;
      next.root = builder.root;
      builder.root.builders.push(next);
      insertOneLevel(next, rest, callback);
    });
  } else {
    var next = TreeBuilder.create();
    next.name = part;
    next.root = builder.root;
    builder.root.builders.push(next);
    insertOneLevel(next, rest, callback);
  }
}