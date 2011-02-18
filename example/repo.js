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

//// This is invalid
//g.git_repository_open('/etc/hosts', function(err, path) {
//    console.log(g.git_strerror(err), path);
//    console.log(g.git_repository_open.toString());
//});
//
//// This is valid
//g.git_repository_open('/home/tim/Dropbox/Projects/TabDeveloper/V4/.git', function(err, path) {
//    console.log(g.git_strerror(err), path);
//});
