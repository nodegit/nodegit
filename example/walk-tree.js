var git = require('../'),
    path = require('path');

// A `tree` in git is typically a representation of the filesystem at
// a revision. A tree has a set of entries, each entry being either a
// tree (directory), or a file.

git.Repo.open(path.resolve(__dirname, '../.git'), function(error, repo) {
  if (error) throw error;

  repo.getMaster(function(error, branch) {
    if (error) throw error;

    branch.getTree(function(error, tree) {
      if (error) throw error;

      // `walk()` returns an event.
      var walker = tree.walk();
      walker.on('entry', function(entry) {
        console.log(entry.path());
      });

      // Don't forget to call `start()`!
      walker.start();
    });
  });
});
