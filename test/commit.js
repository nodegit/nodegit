/**
 * Test that getDiff works as expected.
 */
exports.getDiff = function(test) {
  test.expect(1);
  git.Repo.open('repos/workdir/.git', function(error, repository) {
    repository.getCommit(historyCountKnownSHA, function(error, commit) {
    });
  });
};
