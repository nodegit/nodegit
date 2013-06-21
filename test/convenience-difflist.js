var git = require('../'),
    rimraf = require('rimraf'),
    fs = require( 'fs' );

var historyCountKnownSHA = 'fce88902e66c72b5b93e75bdb5ae717038b221f6';

// Helper functions
var helper = {
  // Test if obj is a true function
  testFunction: function(test, obj, label) {
    // The object reports itself as a function
    test(typeof obj, 'function', label + ' reports as a function.');
    // This ensures the repo is actually a derivative of the Function [[Class]]
    test(toString.call(obj), '[object Function]', label + ' [[Class]] is of type function.');
  },
  // Test code and handle exception thrown
  testException: function(test, fun, label) {
    try {
      fun();
      test(false, label);
    }
    catch (ex) {
      test(true, label);
    }
  }
};

/**
 * Test that the commit object is present.
 */
exports.method = function(test){
  test.expect(2);
  helper.testFunction(test.equals, git.diffList, 'DiffList');
  test.done();
};

/**
 * Test that retreiving parent works as expected.
 *
 * @param  {Object} test
 */
exports.walkingDiffs = function(test) {
  test.expect(15);
  git.repo('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      commit.parents(function(error, parents) {
        var parentSha = parents[0].sha();
        (new git.diffList(commit.rawRepo)).treeToTree(parentSha, historyCountKnownSHA, function(error, diffList) {
          test.equal(null, error, 'Should not error');
          diffList.walk().on('delta', function(error, delta) {
            test.equal(null, error, 'Should not error');
            test.equal(delta.oldFile.path, 'README.md', 'Old file path should match expected');
            test.equal(delta.newFile.path, 'README.md', 'New file path should match expected');
            test.equal(delta.content.length, 5, 'Content array should be of known length');
            test.equal(delta.status, diffList.deltaTypes.GIT_DELTA_MODIFIED, 'Status should be known type');
            test.equal(delta.content[0].lineOrigin, diffList.lineOriginTypes.GIT_DIFF_LINE_CONTEXT, 'First content item should be context');
            test.equal(delta.content[1].lineOrigin, diffList.lineOriginTypes.GIT_DIFF_LINE_CONTEXT, 'Second content item should be context');
            test.equal(delta.content[2].lineOrigin, diffList.lineOriginTypes.GIT_DIFF_LINE_CONTEXT, 'Third content item should be context');

            var oldContent = '__Before submitting a pull request, please ensure both unit tests and lint checks pass.__\n';
            test.equal(delta.content[3].content, oldContent, 'Old content should match known value');
            test.equal(delta.content[3].lineOrigin, diffList.lineOriginTypes.GIT_DIFF_LINE_DELETION, 'Fourth content item should be deletion');
            test.equal(delta.content[3].contentLength, 90, 'Fourth content length should match known value');

            var newContent = '__Before submitting a pull request, please ensure both that you\'ve added unit tests to cover your shiny new code, and that all unit tests and lint checks pass.__\n';
            test.equal(delta.content[4].content, newContent, 'New content should match known value');
            test.equal(delta.content[4].lineOrigin, diffList.lineOriginTypes.GIT_DIFF_LINE_ADDITION, 'Fifth content item should be addition');
            test.equal(delta.content[4].contentLength, 162, 'Fifth content length should match known value');
            test.done();
          });
        });
      });
    });
  });
};

exports.walkingEnd = function(test) {
  test.expect(2);
  git.repo('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      commit.parents(function(error, parents) {
        var parentSha = parents[0].sha();
        (new git.diffList(commit.rawRepo)).treeToTree(parentSha, historyCountKnownSHA, function(error, diffList) {
          diffList.walk().on('end', function(error, diffs) {
            test.equal(null, error, 'Should not error');
            test.equal(diffs.length, 1, 'Diffs array should be of known length');
            test.done();
          });
        });
      });
    });
  });
};

exports.deltaTypes = function(test) {
  test.expect(9);
  git.repo('../.git', function(error, repo) {
    var diffList = new git.diffList(repo.rawRepo);
    test.equal(diffList.deltaTypes.GIT_DELTA_UNMODIFIED, git.raw.DiffList.deltaTypes.GIT_DELTA_UNMODIFIED, 'GIT_DELTA_UNMODIFIED delta type should match expected value');
    test.equal(diffList.deltaTypes.GIT_DELTA_ADDED, git.raw.DiffList.deltaTypes.GIT_DELTA_ADDED, 'GIT_DELTA_ADDED delta type should match expected value');
    test.equal(diffList.deltaTypes.GIT_DELTA_DELETED, git.raw.DiffList.deltaTypes.GIT_DELTA_DELETED, 'GIT_DELTA_DELETED delta type should match expected value');
    test.equal(diffList.deltaTypes.GIT_DELTA_MODIFIED, git.raw.DiffList.deltaTypes.GIT_DELTA_MODIFIED, 'GIT_DELTA_MODIFIED delta type should match expected value');
    test.equal(diffList.deltaTypes.GIT_DELTA_RENAMED, git.raw.DiffList.deltaTypes.GIT_DELTA_RENAMED, 'GIT_DELTA_RENAMED delta type should match expected value');
    test.equal(diffList.deltaTypes.GIT_DELTA_COPIED, git.raw.DiffList.deltaTypes.GIT_DELTA_COPIED, 'GIT_DELTA_COPIED delta type should match expected value');
    test.equal(diffList.deltaTypes.GIT_DELTA_IGNORED, git.raw.DiffList.deltaTypes.GIT_DELTA_IGNORED, 'GIT_DELTA_IGNORED delta type should match expected value');
    test.equal(diffList.deltaTypes.GIT_DELTA_UNTRACKED, git.raw.DiffList.deltaTypes.GIT_DELTA_UNTRACKED, 'GIT_DELTA_UNTRACKED delta type should match expected value');
    test.equal(diffList.deltaTypes.GIT_DELTA_TYPECHANGE, git.raw.DiffList.deltaTypes.GIT_DELTA_TYPECHANGE, 'GIT_DELTA_TYPECHANGE delta type should match expected value');
    test.done();
  });
};

exports.lineOriginTypes = function(test) {
  test.expect(8);
  git.repo('../.git', function(error, repo) {
    var diffList = new git.diffList(repo.rawRepo);
    test.equal(diffList.lineOriginTypes.GIT_DIFF_LINE_CONTEXT, git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_CONTEXT, 'GIT_DIFF_LINE_CONTEXT line origin type should match expected value');
    test.equal(diffList.lineOriginTypes.GIT_DIFF_LINE_ADDITION, git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_ADDITION, 'GIT_DIFF_LINE_ADDITION line origin type should match expected value');
    test.equal(diffList.lineOriginTypes.GIT_DIFF_LINE_DELETION, git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_DELETION, 'GIT_DIFF_LINE_DELETION line origin type should match expected value');
    test.equal(diffList.lineOriginTypes.GIT_DIFF_LINE_ADD_EOFNL, git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_ADD_EOFNL, 'GIT_DIFF_LINE_ADD_EOFNL line origin type should match expected value');
    test.equal(diffList.lineOriginTypes.GIT_DIFF_LINE_DEL_EOFNL, git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_DEL_EOFNL, 'GIT_DIFF_LINE_DEL_EOFNL line origin type should match expected value');
    test.equal(diffList.lineOriginTypes.GIT_DIFF_LINE_FILE_HDR, git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_FILE_HDR, 'GIT_DIFF_LINE_FILE_HDR line origin type should match expected value');
    test.equal(diffList.lineOriginTypes.GIT_DIFF_LINE_HUNK_HDR, git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_HUNK_HDR, 'GIT_DIFF_LINE_HUNK_HDR line origin type should match expected value');
    test.equal(diffList.lineOriginTypes.GIT_DIFF_LINE_BINARY, git.raw.DiffList.lineOriginTypes.GIT_DIFF_LINE_BINARY, 'GIT_DIFF_LINE_BINARY line origin type should match expected value');
    test.done();
  });
};
