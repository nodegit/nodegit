// Load in the module.
var git = require('nodegit');

// Open the repository in the current directory.
git.repo('.git', function(error, repository) {
  if (error) throw error;

  // Use the master branch.
  repository.branch('master', function(error, branch) {
    if (error) throw error;

    // Iterate over the revision history.
    branch.tree(function(error, tree) {
        console.log(tree);
      if (error) throw error;
      tree.walk().on('entry', function(error, entry) {
        entry.name(function(error, name) {
            console.log(name);
        });
      });
    });
  });
});
