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
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      commit.parents(function(error, parents) {
        var parentSha = parents[0].sha();
        git.diffList.treeToTree(commit.repo, parentSha, historyCountKnownSHA, function(error, diffList) {
          test.equal(null, error, 'Should not error');
          diffList.walk().on('delta', function(error, delta) {
            test.equal(null, error, 'Should not error');
            test.equal(delta.oldFile.path, 'README.md', 'Old file path should match expected');
            test.equal(delta.newFile.path, 'README.md', 'New file path should match expected');
            test.equal(delta.content.length, 5, 'Content array should be of known length');
            test.equal(delta.status, git.diffList.Delta.Modified, 'Status should be known type');
            test.equal(delta.content[0].lineOrigin, git.diffList.LineOrigin.Context, 'First content item should be context');
            test.equal(delta.content[1].lineOrigin, git.diffList.LineOrigin.Context, 'Second content item should be context');
            test.equal(delta.content[2].lineOrigin, git.diffList.LineOrigin.Context, 'Third content item should be context');

            var oldContent = '__Before submitting a pull request, please ensure both unit tests and lint checks pass.__\n';
            test.equal(delta.content[3].content, oldContent, 'Old content should match known value');
            test.equal(delta.content[3].lineOrigin, git.diffList.LineOrigin.Deletion, 'Fourth content item should be deletion');
            test.equal(delta.content[3].contentLength, 90, 'Fourth content length should match known value');

            var newContent = '__Before submitting a pull request, please ensure both that you\'ve added unit tests to cover your shiny new code, and that all unit tests and lint checks pass.__\n';
            test.equal(delta.content[4].content, newContent, 'New content should match known value');
            test.equal(delta.content[4].lineOrigin, git.diffList.LineOrigin.Addition, 'Fifth content item should be addition');
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
  git.repo.open('../.git', function(error, repository) {
    repository.commit(historyCountKnownSHA, function(error, commit) {
      commit.parents(function(error, parents) {
        var parentSha = parents[0].sha();
        git.diffList.treeToTree(commit.repo, parentSha, historyCountKnownSHA, function(error, diffList) {
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
