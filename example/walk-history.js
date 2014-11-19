var nodegit = require('../'),
    path = require('path'),
    sort = nodegit.Revwalk.Sort;

// This code walks the history of the master branch and prints results
// that look very similar to calling `git log` from the command line

nodegit.Repository.open(path.resolve(__dirname, '../.git')).then(function(repo) {
  return repo.getMaster();
}).then(function(branch){
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
}).done();
