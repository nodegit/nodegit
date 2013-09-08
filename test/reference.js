var git = require('../'),
    rimraf = require('rimraf');

// Ref::Lookup
exports.lookup = function(test) {
  test.expect(1);

  git.Repo.open('repos/workdir/.git', function(error, repo) {
    repo.getReference('refs/heads/master', function(error, reference) {
      test.ok(reference instanceof git.Reference);
      test.done();
    });
  });
};
