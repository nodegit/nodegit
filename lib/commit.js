var git = require( '../' ),
  success = require('./utilities').success,
  events = require('events');

/**
 * Convenience commit constructor.
 *
 * @constructor
 * @param  {RawCommit|Null} rawCommit
 * @return {Commit}
 */
var Commit = function(rawRepo, rawCommit) {
  if (!(rawRepo instanceof git.raw.Repo)) {
    throw git.error('First parameter for Commit must be a raw repo');
  }

  this.rawRepo = rawRepo;

  if (rawCommit instanceof git.raw.Commit) {
    this.rawCommit = rawCommit;
  } else {
    this.rawCommit = new git.raw.Commit();
  }
};

/**
 * Look up the commit referenced by oid, replace this.commit with the result.
 *
 * @param  {git.oid|git.raw.Oid|String} oid
 * @param  {Function} callback
 */
Commit.prototype.lookup = function(oid, callback) {
  if (typeof oid !== 'undefined' &&
      typeof oid !== 'string' &&
      !(oid instanceof git.raw.Oid)) {
    oid = oid.getRawOid();
  }
  var self = this;
  self.rawCommit.lookup(self.rawRepo, oid, function commitLookup(error, rawCommit) {
    if (success(error, callback)) {
      self.rawCommit = rawCommit;
      callback(null, self);
    }
  });
};

/**
 * Retrieve the commit's Oid
 *
 * @param  {Function} callback
 */
Commit.prototype.oid = function(callback) {
  callback(null, new git.oid(this.rawCommit.oid()));
};

/**
 * Retrieve the SHA
 *
 * @param  {Function} callback
 */
Commit.prototype.sha = function(callback) {
  this.rawCommit.sha(function(error, sha) {
    if (success(error, callback)) {
      callback(null, sha);
    }
  });
};

/**
 * Retrieve the message
 *
 * @param  {Function} callback
 */
Commit.prototype.message = function(callback) {
  this.rawCommit.message(function(error, message) {
    if (success(error, callback)) {
      callback(null, message);
    }
  });
};

/**
 * Retrieve the commit unix timestamp.
 *
 * @param  {Function} callback
 */
Commit.prototype.time = function(callback) {
  this.rawCommit.time(function(error, time) {
    if (success(error, callback)) {
      callback(null, time);
    }
  });
};

/**
 * Retrieve the commit's positive or negative timezone offset, in minutes from UTC.
 *
 * @param  {Function} callback
 */
Commit.prototype.offset = function(callback) {
  this.rawCommit.offset(function(error, offset) {
    if (success(error, callback)) {
      callback(null, offset);
    }
  });
};

/**
 * Retrieve the commit's author.
 *
 * @param  {Function} callback
 */
Commit.prototype.author = function(callback) {
  this.rawCommit.author(function(error, rawSignature) {
    if (success(error, callback)) {
      callback(null, new git.raw.signature(rawSignature));
    }
  });
};

/**
 * Retrieve the commit's committer.
 *
 * @param  {Function} callback
 */
Commit.prototype.committer = function(callback) {
  this.rawCommit.committer(function(error, rawSignature) {
    if (success(error, callback)) {
      callback(null, new git.raw.signature(rawSignature));
    }
  });
};
/**
 * Retrieve the tree for this commit.
 *
 * @param  {Function} callback
 */
Commit.prototype.tree = function(callback) {
  var self = this;
  self.rawCommit.tree(function commitTree(error, rawTree) {
    if (success(error, callback)) {
      callback(null, new git.tree(self.rawRepo, rawTree));
    }
  });
};

/**
 * Retrieve the author of this commit.
 *
 * @param  {Function} callback
 */
Commit.prototype.tree = function(callback) {
  this.rawCommit.author(function(error, author) {
    if (success(error, callback)) {
      callback(null, author);
    }
  });
};

/**
 * Retrieve the file represented by path for this commit.
 *
 * @param  {String} path
 * @param  {Function} callback
 */
Commit.prototype.file = function(path, callback) {
  this.tree(function commitFile(error, tree) {
    if (!success(error, callback)) {
      return;
    }
    tree.entry(path, function(error, entry) {
      if (success(error, callback)) {
        callback(null, entry);
      }
    });
  });
};

/**
 * Walk the history of this commit.
 *
 * @return {Event} Event emits 'commit', with error, commit and 'end', with
 *                       error, commits[]
 */
Commit.prototype.history = function() {
  var event = new events.EventEmitter(),
      self = this;

  self.oid(function commitOid(error, oid) {
    var revwalk = new git.revwalk(self.rawRepo),
        commits = [];
    revwalk.walk(oid, function commitRevWalk(error, index, commit, noMoreCommits) {
      if(error) {
        event.emit('end', error, commits);
        return false;
      }

      if (noMoreCommits) {
        event.emit('end', null, commits);
        return;
      }
      event.emit('commit', null, commit);
      commits.push(commit);
    });
  });

  return event;
};

/**
 * Retrieve the commit's parent at the given position asynchronously.
 *
 * @param  {Integer} position
 */
Commit.prototype.parent = function(position, callback) {
  self.rawCommit.parent(position, function processParent(error, rawParent) {
    if (success(error, callback)) {
      callback(null, new Commit(this.rawRepo, rawParent));
    }
  });
};

/**
 * Get a diff tree showing changes between this commit and its parent(s).
 *
 * @param  {Function} callback
 */
Commit.prototype.parentsDiffTrees = function(callback) {
  var self = this;
  self.sha(function(error, commitSha) {
    if (!success(error, callback)) {
      return;
    }
    self.parents(function commitParents(error, parents) {
      if (!success(error, callback)) {
        return;
      }
      var parentDiffLists = [];
      parents.forEach(function commitEachParent(parent) {
        parent.sha(function commitParentSha(error, parentSha) {
          (new git.diffList(self.repo)).treeToTree(parentSha, commitSha, function walkDiffList(error, diffList) {
            if (!success(error, callback)) {
              return;
            }
            parentDiffLists.push(diffList);
            if (parentDiffLists.length === parents.length) {
              callback(null, parentDiffLists);
            }
          });
        });
      });
    });
  });
};

exports.commit = Commit;
