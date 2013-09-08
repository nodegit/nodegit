var git = require('../'),
    path = require('path');

exports.content = function(test) {
  var testOid = git.Oid.fromString('111dd657329797f6165f52f5085f61ac976dcf04');
  test.expect(1);
  git.Repo.open(path.resolve('repos/workdir/.git'), function(err, repo) {
    repo.getBlob(testOid, function(err, blob) {
      test.equals(blob.toString().slice(0, 7), "@import");
      test.done();
    });
  });
};
