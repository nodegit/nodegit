var git = require('../'),
    events = require('events');

var GitDiffList = function(rawRepo, rawDiffList) {
  var self = {
    repo: rawRepo,
    diffList: rawDiffList || new git.raw.DiffList()
  };

  self.walk = function() {
    var event = new events.EventEmitter(),
        allFileDeltas = [];

    process.nextTick(function() {
      self.diffList.walk(function fileCallback(error, fileDeltas) {
        if (error) {
          event.emit('end', error, null);
        }
        fileDeltas.forEach(function(fileDelta) {
          event.emit('file', null, fileDelta);
          allFileDeltas.push(fileDelta);
        })
      }, function hunkCallback(error, diffHunk) {
        // @todo implement?
      }, function lineCallback(error, diffLine) {
        // @todo implement?
      }, function endCallback(error) {
        event.emit('end', error, allFileDeltas);
      });
    })

    return event;
  };

  self.treeToTree = function(oldSha, newSha, callback) {
    self.diffList.treeToTree(self.repo, oldSha, newSha, function(error, rawDifflist) {
      if (error) {
        callback(error, null);
        return;
      }
      self.diffList = rawDifflist;
      callback(null, self);
    });
  };

  /**
   * Add libgit2 delta types to git.diffList object.
   *
   * Refer to vendor/libgit2/include/git2/diff.h for definitions.
   */
  for (var deltaType in git.raw.DiffList.deltaTypes) {
    self[deltaType] = git.raw.DiffList.deltaTypes[deltaType];
  }

  /**
   * Add libgit2 line origin types to git.diffList object.
   *
   * Refer to vendor/libgit2/include/git2/diff.h for definitions.
   */
  for (var lineOriginType in git.raw.DiffList.lineOriginTypes) {
    self[lineOriginType] = git.raw.DiffList.lineOriginTypes[lineOriginType];
  }

  return self;
};

exports.diffList = GitDiffList;
