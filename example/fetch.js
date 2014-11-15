var git = require('../');
var path = require('path');

git.Repository.open(path.resolve(__dirname, '../.git'))
.then(function(repo) {
  return git.Remote.load(repo, "origin");
})
.then(function(remote) {
  remote.connect(0);
  return remote.download();
})
.done(function() {
  console.log("It worked!");
});
