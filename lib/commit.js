var git = require( '../' ),
  success = require('./utilities').success,
  events = require('events');

/**
 * Convenience commit constructor.
 *
 * @constructor
 * @param {git.raw.Repo} rawRepo Raw repository object.
 * @param  {git.raw.Commit} [rawCommit = new git.raw.Commit(rawRepo)] Raw commit object.
 */
var Commit = function(rawRepo, rawCommit) {
  if (!(rawRepo instanceof git.raw.Repo)) {
    throw new git.error('First parameter for Commit must be a raw repo');
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
 * @param {Oid|git.raw.Oid|String} oid A representation of an OID used to lookup the commit.
 * @param {Commit~lookupCallback} callback
 */
Commit.prototype.lookup = function(oid, callback) {
  /**
   * @callback Commit~lookupCallback Callback executed on lookup completion.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Commit|null} commit Retrieved commit object or null.
   */
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
 * Retrieve the commit's OID.
 *
 * @param {Commit~oidCallback} callback
 */
Commit.prototype.oid = function() {
  /**
   * @callback Commit~oidCallback Callback executed on OID retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Oid|null} commit Retrieved OID object or null.
   */
  return new git.oid(this.rawCommit.oid());
};

/**
 * Retrieve the SHA.
 */
Commit.prototype.sha = function() {
  return this.oid().sha();
};

/**
 * Retrieve the message
 *
 * @param {Commit~messageCallback} callback
 */
Commit.prototype.message = function(callback) {
  /**
   * @callback Commit~messageCallback Callback executed on message retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {String|null} message Retrieved message.
   */
  this.rawCommit.message(function(error, message) {
    if (success(error, callback)) {
      callback(null, message);
    }
  });
};

/**
 * Retrieve the commit time as a unix timestamp.
 *
 * @param {Commit~timeCallback} callback
 */
Commit.prototype.time = function(callback) {
  /**
   * @callback Commit~timeCallback Callback executed on time retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Integer|null} time Retrieved time in seconds.
   */
  this.rawCommit.time(function(error, time) {
    if (success(error, callback)) {
      // git_commit_time returns timestamp in s, converting to ms here
      callback(null, time * 1000);
    }
  });
};

/**
 * Retrieve the commit time as a Date object.
 *
 * @param {Commit~dateCallback} callback
 */
Commit.prototype.date = function(callback) {
  /**
   * @callback Commit~dateCallback Callback executed on date retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Date|null} time Retrieved time as a Date object.
   */
  this.time(function(error, time) {
    if (success(error, callback)) {
      callback(null, new Date(time));
    }
  });
};

/**
 * Retrieve the commit's positive or negative timezone offset, in minutes from UTC.
 *
 * @param {Commit~offsetCallback} callback
 */
Commit.prototype.offset = function(callback) {
  /**
   * @callback Commit~offsetCallback Callback executed on offset retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Integer|null} offset Retrieved offset in in minutes from UTC.
   */
  this.rawCommit.offset(function(error, offset) {
    if (success(error, callback)) {
      callback(null, offset);
    }
  });
};

/**
 * Retrieve the commit's author signature.
 *
 * @param {Commit~authorCallback} callback
 */
Commit.prototype.author = function(callback) {
  /**
   * @callback Commit~authorCallback Callback executed on author retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Signature|null} author Retrieved author signature.
   */
  this.rawCommit.author(function(error, rawSignature) {
    if (success(error, callback)) {
      callback(null, new git.signature(rawSignature));
    }
  });
};

/**
 * Retrieve the commit's committer.
 *
 * @param {Commit~committerCallback} callback
 */
Commit.prototype.committer = function(callback) {
  /**
   * @callback Commit~committerCallback Callback executed on committer retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Signature|null} committer Retrieved committer signature.
   */
  this.rawCommit.committer(function(error, rawSignature) {
    if (success(error, callback)) {
      callback(null, new git.signature(rawSignature));
    }
  });
};

/**
 * Retrieve the tree for this commit.
 *
 * @param {Commit~treeCallback} callback
 */
Commit.prototype.tree = function(callback) {
  /**
   * @callback Commit~treeCallback Callback executed on tree retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Tree|null} tree Retrieved tree.
   */
  var self = this;
  self.rawCommit.tree(function commitTree(error, rawTree) {
    if (success(error, callback)) {
      callback(null, new git.tree(self.rawRepo, rawTree));
    }
  });
};

/**
 * Retrieve the file represented by path for this commit.
 * Path must be relative to repository root.
 *
 * @param {String} path
 * @param {Commit~fileCallback} callback
 */
Commit.prototype.file = function(path, callback) {
  /**
   * @callback Commit~fileCallback Callback executed on file retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Entry|null} file Retrieved file entry.
   */
  this.tree(function commitFileCallback(error, tree) {
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
 * Walk the history from this commit backwards.
 * An EventEmitter is returned that will emit a 'commit' event for each 
 * commit in the history, and one 'end' event when the walk is completed.
 *
 * @fires Commit#commit
 * @fires Commit#end
 *
 * @return {EventEmitter} historyWalkEmitter
 */
Commit.prototype.history = function() {
  var event = new events.EventEmitter(),
      self = this;

  var oid = self.oid();
  (new git.revwalk(self.rawRepo)).allocate(function createRevwalk(error, revwalk) {
    var commits = [];
    revwalk.walk(oid, function commitRevWalk(error, index, commit, noMoreCommits) {
      if(error) {
        event.emit('end', error, commits);
        return false;
      }

      if (noMoreCommits) {
        /**
         * End event.
         *
         * @event Commit#end
         *
         * @param {GitError|null} error An error object if there was an issue, null otherwise.
         * @param {Commit[]} commits The commits.
         */
        event.emit('end', null, commits);
        return;
      }
      /**
       * Commit event.
       *
       * @event Commit#commit
       *
       * @param {GitError|null} error An error object if there was an issue, null otherwise.
       * @param {Commit} commit The commit.
       */
      event.emit('commit', null, commit);
      commits.push(commit);
    });
  });
  return event;
};

/**
 * Retrieve the commit's parents.
 *
 * @param {Commit~parentsCallback} callback
 */
Commit.prototype.parents = function(callback) {
  /**
   * @callback Commit~parentsCallback Callback executed on parents retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {Commit[]|null} parents Commit's parent(s).
   */
  var self = this;
  self.rawCommit.parents(function processParent(error, rawParents) {
    if (success(error, callback)) {
      var parents = [];
      rawParents.forEach(function eachParent(rawParent) {
        parents.push(new Commit(self.rawRepo, rawParent));
      });
      callback(null, parents);
    }
  });
};

/**
 * Generate an array of diff trees showing changes between this commit 
 * and its parent(s).
 *
 * @param {Commit~parentsDiffTreesCallback} callback
 */
Commit.prototype.parentsDiffTrees = function(callback) {
  /**
   * @callback Commit~parentsDiffTreesCallback Callback executed on diff trees retrieval.
   * @param {GitError|null} error An Error or null if successful.
   * @param {DiffList[]|null} diffLists Array of DiffTrees showing changes between this commit and its parent(s)
   */
  var self = this;
  var commitSha = self.sha();
  self.parents(function commitParents(error, parents) {
    if (!success(error, callback)) {
      return;
    }
    var parentDiffLists = [];
    parents.forEach(function commitEachParent(parent) {
      var parentSha = parent.sha();
      (new git.diffList(self.rawRepo)).treeToTree(parentSha, commitSha, function walkDiffList(error, diffList) {
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
};

exports.commit = Commit;
