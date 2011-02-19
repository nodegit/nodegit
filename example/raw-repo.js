var git2 = require('../build/default/git2');

var repo = new git2.Repo();

// Creating a git repo
repo.init('./.git', true, function(err, path, is_bare) {
    console.log("Is the repo created bare?", is_bare);
    // Access existing repository
    repo.open('./.git', function(err, path) {
        console.log(err, path);
    });
});
