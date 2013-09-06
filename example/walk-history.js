var git = require('../'),
    path = require('path'),
    sort = git.RevWalk.Sort;

// This code walks the history of the master branch and prints results
// that look very similar to calling `git log` from the command line

git.Repo.open(path.resolve(__dirname, '../.git'), function(error, repo) {
  if (error) throw error;

  repo.getMaster(function(error, branch) {
    if (error) throw error;

    // History returns an event.
    var history = branch.history(sort.Time);

    // History emits 'commit' event for each commit in the branch's history
    history.on('commit', function(commit) {
      console.log('commit ' + commit.sha());
      console.log('Author:', commit.author().name() + ' <' + commit.author().email() + '>');
      console.log('Date:', commit.date());
      console.log('\n    ' + commit.message());
    });

    // Don't forget to call `start()`!
    history.start();
  });
});
