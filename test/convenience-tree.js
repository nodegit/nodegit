var git = require('../'),
    rimraf = require('rimraf'),
    fs = require('fs');

var sha = '5716e9757886eaf38d51c86b192258c960d9cfea';
var fileCount = 512; // Number of blob & blob executabless

exports.walk = function(test) {
  test.expect(516);

  git.repo('../.git', function(error, repo) {
    repo.commit(sha, function(error, commit) {
      var entryCount = 0;
      commit.tree(function(error, tree) {
        tree.walk().on('entry', function(error, index, entry) {
            test.equals(error, null, 'There should be no error');
            entryCount++;
        }).on('end', function(error, entries) {
          test.equals(error, null, 'There should be no error');
          test.equals(entryCount, fileCount, 'The manual tree entry count and the "end" tree entry count do not match');
          test.equals(entries.length, fileCount, 'The end entries count and the manual entry count do not match');
          test.done();
        });
      });
    });
  });
};
