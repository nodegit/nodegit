// Load in the module.
var git = require('nodegit'),
  async = require('async');

// Open the repository in the current directory.
git.repo('.git', function(error, repository) {
  if (error) throw error;

  // Use the master branch (a branch is the HEAD commit)
  repository.branch('master', function(error, branch) {
    if (error) throw error;

    // History returns an event, and begins walking the history
    var history = branch.history();

    // History emits 'commit' event for each commit in the branch's history
    history.on('commit', function(error, commit) {
      // Print out `git log` emulation.
        async.series([
            function(callback) {
                commit.sha(callback);
            },
            function(callback) {
                commit.date(callback);
            },
            function(callback) {
                commit.author(function(error, author) {
                    author.name(callback);
                });
            },
            function(callback) {
                commit.author(function(error, author) {
                    author.email(callback);
                });
            },
            function(callback) {
                commit.message(callback);
            }
        ], function printCommit(error, results) {
            if (error) throw error;
            console.log('SHA ' + results[0]);
            console.log(results[1] * 1000);
            console.log(results[2] + ' <' + results[3] + '>');
            console.log(results[4]);
        });
    });
  });
});
