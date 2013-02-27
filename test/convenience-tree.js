var git = require('../');
var rimraf = require('rimraf');
var fs = require('fs');

var sha = '5716e9757886eaf38d51c86b192258c960d9cfea';
var fileCount = 513;

exports.walk = function(test) {
  test.expect(1);

  git.repo('../.git', function(error, repo) {
    if(error) { throw error; }
      // @todo assert repo is correct
      repo.commit(sha, function(error, commit) {
        if(error) { throw error; }
        var entryCount = 0;
        commit.tree().walk().on('entry', function(index, entry) {
            entryCount++;
        }).on('end', function() {
            test.equals(entryCount, fileCount, 'The manual tree entry count and the "end" tree entry count do not match');
            test.done();
        });
      });
   });
};
