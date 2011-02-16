var git2 = require('../build/default/git2');

var g = new git2.Git2();

// This is invalid
g.repo('/etc/hosts', function(err, path) {
    console.log(err, path);
});

// This is valid
g.repo('/home/tim/Dropbox/Projects/TabDeveloper/V4/.git', function(err, path) {
    console.log(err, path);
});
