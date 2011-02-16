var git2 = require('../build/default/git2');

var g = new git2.Git2();

// This is invalid
g.git_repository_open('/etc/hosts', function(err, path) {
    console.log(g.git_strerror(err), path);
});

// This is valid
g.git_repository_open('/home/tim/Dropbox/Projects/TabDeveloper/V4/.git', function(err, path) {
    console.log(g.git_strerror(err), path);
});
