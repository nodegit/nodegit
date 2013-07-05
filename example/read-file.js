var git = require('../'),
    path = require('path');

// This example opens a certain file, `README.md`, at a particular commit,
// and prints the first 10 lines as well as some metadata.

git.Repo.open(path.resolve(__dirname, '../.git'), function(error, repo) {
  if (error) throw error;

  repo.getCommit('59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5', function(error, commit) {
    if (error) throw error;

    commit.getEntry('README.md', function(error, entry) {
      if (error) throw error;

      entry.getBlob(function(error, blob) {
        if (error) throw error;

        console.log(entry.name(), entry.sha(), blob.size() + 'b');
        console.log('========================================================\n\n');
        var firstTenLines = blob.toString().split('\n').slice(0, 10).join('\n');
        console.log(firstTenLines);
        console.log('...');
      });
    });
  });
});
