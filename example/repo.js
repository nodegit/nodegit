var git2 = require('../build/default/git2');

var repo = new git2.Repo();

// Creating a git repo
repo.init('./.git', false, function(err, path) {
    // Access existing repository
    repo.open('./.git', function(err, path) {
        console.log(err, path);
    });
});
