var nodegit = require('../'),
    path = require('path');

// A `tree` in git is typically a representation of the filesystem at
// a revision. A tree has a set of entries, each entry being either a
// tree (directory), or a file.

nodegit.Repository.open(path.resolve(__dirname, '../.git')).then(function(repo) {
  return repo.getMaster();
}).then(function(branch) {
    return branch.getTree();
}).then(function(tree) {
  // `walk()` returns an event.
  var walker = tree.walk();
  walker.on('entry', function(entry) {
    console.log(entry.path());
  });

  // Don't forget to call `start()`!
  walker.start();
}).done();
