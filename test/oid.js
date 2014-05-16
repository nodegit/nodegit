var git = require('../');

var knownSha = 'fce88902e66c72b5b93e75bdb5ae717038b221f6';

exports.fromStringAndSha = function(test) {
  test.expect(1);
  var oid = git.Oid.fromstr(knownSha);
  test.equal(oid.allocfmt(), knownSha, 'SHA should match known value');
  test.done();
};
