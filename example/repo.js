var git2 = require('../build/default/git2');

var g = new git2.Git2();

// This is invalid
console.log(g.repo('/etc/hosts'));

// This is valid
console.log(g.repo('/home/tim/Dropbox/Projects/TabDeveloper/V4/.git'));
