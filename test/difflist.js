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
  git.Repo.open('../.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      commit.getParents(function(error, parents) {
        parents[0].getTree(function(error, parentTree) {
          commit.getTree(function(error, commitTree) {
            parentTree.diff(commitTree, function(error, diffList) {
              test.equal(null, error, 'Should not error');

              diffList.patches().forEach(function(patch) {
                var delta = patch.delta, patch = patch.patch;
                test.equal(null, error, 'Should not error');
                test.equal(delta.oldFile().path(), 'README.md', 'Old file path should match expected');
                test.equal(delta.newFile().path(), 'README.md', 'New file path should match expected');
                test.equal(patch.size(), 1, 'Content array should be of known length');
                var hunk = patch.hunk(0);

                test.equal(hunk.lines, 5, 'Content array should be of known length');
                test.equal(delta.status(), git.DiffList.Delta.Modified, 'Status should be known type');

                test.equal(patch.line(0, 0).lineOrigin, git.DiffList.LineOrigin.Context, 'First content item should be context');
                test.equal(patch.line(0, 1).lineOrigin, git.DiffList.LineOrigin.Context, 'Second content item should be context');
                test.equal(patch.line(0, 2).lineOrigin, git.DiffList.LineOrigin.Context, 'Third content item should be context');

                var oldContent = '__Before submitting a pull request, please ensure both unit tests and lint checks pass.__\n';
                test.equal(patch.line(0, 3).content, oldContent, 'Old content should match known value');
                test.equal(patch.line(0, 3).lineOrigin, git.DiffList.LineOrigin.Deletion, 'Fourth content item should be deletion');
                test.equal(patch.line(0, 3).length, 90, 'Fourth content length should match known value');

                var newContent = '__Before submitting a pull request, please ensure both that you\'ve added unit tests to cover your shiny new code, and that all unit tests and lint checks pass.__\n';
                test.equal(patch.line(0, 4).content, newContent, 'New content should match known value');
                test.equal(patch.line(0, 4).lineOrigin, git.DiffList.LineOrigin.Addition, 'Fifth content item should be addition');
                test.equal(patch.line(0, 4).length, 162, 'Fifth content length should match known value');
                test.done();
              });
            });
          });
        });
      });
    });
  });
};
