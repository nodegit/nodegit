var git = require('../');

var sha = '5716e9757886eaf38d51c86b192258c960d9cfea';

var getEntry = function(callback) {
  git.repo('../.git', function(error, repo) {
    repo.commit(sha, function(error, commit) {
      commit.file('README.md', function(error, entry) {
        callback(error, entry);
      });
    });
 });
};

exports.sha = function(test) {
  test.expect(1);
  getEntry(function(error, entry) {
    entry.sha(function(error, sha) {
      test.equal(sha, '6cb45ba5d32532bf0d1310dc31ca4f20f59964bc', 'Entry SHA should match expected value');
      test.done();
    });
  });
};
