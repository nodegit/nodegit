var git2 = require('../build/default/git2');

// Access existing repository
var repo = new git2.Repo();
repo.open('./.git', function(err, path) {
    console.log(err, path);
});

// Creating a git repo
repo.init('./test.git', true, function(err, path) {
    console.log(err, path);
});
