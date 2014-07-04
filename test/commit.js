/**
 * Test that retrieving and walking a commit's tree works as expected.
 */
exports.tree = function(test) {
  test.expect(2);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      test.equals(error, null, 'Getting latest branch commit should not error');

      var commitTreeEntryCount = 0;
      var expectedCommitTreeEntryCount = 198;
      commit.getTree(function(error, tree) {
        tree.walk().on('entry', function(entry) {
          commitTreeEntryCount++;
        }).on('end', function(error, entries) {
          test.equals(commitTreeEntryCount, expectedCommitTreeEntryCount, 'Commit tree entry count does not match expected');
          test.done();
        }).start();
      });
    });
  });
};

/**
 * Test that getDiff works as expected.
 */
exports.getDiff = function(test) {
  test.expect(1);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
      commit.getDiff(function(error, diff) {
        test.equals(diff.length, 1, 'Should be one item in parents diff trees');
        test.done();
      });
    });
  });
};

process.on('uncaughtException', function(err) {
  console.log(err.stack);
});
