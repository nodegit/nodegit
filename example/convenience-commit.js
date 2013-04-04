// Load in the module.
var git = require('nodegit'),
    async = require('async');

// Open the repository in the current directory.
git.repo('.git', function(error, repository) {
  if (error) {
    throw error;
  }

  // Use the master branch.
  repository.branch('master', function(error, branch) {
    if (error) {
      throw error;
    }

    // Iterate over the revision history.
    branch.history().on('commit', function(error, commit) {

      // Print out `git log` emulation.
        async.series([
            function(callback) {
                commit.sha(callback);
            },
            function(callback) {
                commit.time(callback);
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
            console.log('SHA ' + results[0]);
            console.log(new Date(results[1] * 1000));
            console.log(results[2] + ' <' + results[3] + '>');
            console.log(results[4]);
        });
    });
  });
});
