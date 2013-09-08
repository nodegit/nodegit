var git = require('../'),
    rimraf = require('rimraf'),
    fs = require( 'fs' );

var historyCountKnownSHA = 'fce88902e66c72b5b93e75bdb5ae717038b221f6';

/**
 * Test that retreiving parent works as expected.
 *
 * @param  {Object} test
 */
exports.walkingDiffs = function(test) {
  test.expect(16);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      commit.getDiff(function(error, diffList) {
        test.equal(null, error, 'Should not error');

        diffList[0].patches().forEach(function(patch) {
          test.equal(null, error, 'Should not error');

          test.equal(patch.oldFile().path(), 'README.md', 'Old file path should match expected');
          test.equal(patch.newFile().path(), 'README.md', 'New file path should match expected');
          test.equal(patch.size(), 1, 'Content array should be of known length');
          test.ok(patch.isModified(), 'Status should be known type');

          var hunk = patch.hunks()[0];
          test.equal(hunk.size(), 5, 'Content array should be of known length');
          var lines = hunk.lines();

          test.equal(lines[0].lineOrigin, git.DiffList.LineOrigin.Context, 'First content item should be context');
          test.equal(lines[1].lineOrigin, git.DiffList.LineOrigin.Context, 'Second content item should be context');
          test.equal(lines[2].lineOrigin, git.DiffList.LineOrigin.Context, 'Third content item should be context');

          var oldContent = '__Before submitting a pull request, please ensure both unit tests and lint checks pass.__\n';
          test.equal(lines[3].content, oldContent, 'Old content should match known value');
          test.equal(lines[3].lineOrigin, git.DiffList.LineOrigin.Deletion, 'Fourth content item should be deletion');
          test.equal(lines[3].length, 90, 'Fourth content length should match known value');

          var newContent = '__Before submitting a pull request, please ensure both that you\'ve added unit tests to cover your shiny new code, and that all unit tests and lint checks pass.__\n';
          test.equal(lines[4].content, newContent, 'New content should match known value');
          test.equal(lines[4].lineOrigin, git.DiffList.LineOrigin.Addition, 'Fifth content item should be addition');
          test.equal(lines[4].length, 162, 'Fifth content length should match known value');
          test.done();
        });
      });
    });
  });
};
