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
