var git = require('../'),
    path = require('path');

// This code examines the diffs between a particular commit and all of its
// parents. Since this commit is not a merge, it only has one parent. This is
// similar to doing `git show`.

git.Repo.open(path.resolve(__dirname, '../.git'), function(error, repo) {
  if (error) throw error;

  repo.getCommit('59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5', function(error, commit) {
    if (error) throw error;

    console.log('commit ' + commit.sha());
    console.log('Author:', commit.author().name() + ' <' + commit.author().email() + '>');
    console.log('Date:', commit.date());
    console.log('\n    ' + commit.message());

    commit.getDiff(function(error, diffList) {
      if (error) throw error;

      diffList.forEach(function(diff) {
        diff.patches().forEach(function(patch) {
          console.log("diff", patch.oldFile().path(), patch.newFile().path());
          patch.hunks().forEach(function(hunk) {
            console.log(hunk.header().trim());
            hunk.lines().forEach(function(line) {
              console.log(String.fromCharCode(line.lineOrigin) + line.content.trim());
            });
          });
        });
      });
    });
  });
});
