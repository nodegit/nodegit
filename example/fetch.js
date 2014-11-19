var nodegit = require('../');
var path = require('path');

nodegit.Repository.open(path.resolve(__dirname, '../.git')).then(function(repo) {
  return nodegit.Remote.load(repo, "origin");
}).then(function(remote) {
  remote.connect(0);
  return remote.download();
})
.done(function() {
  console.log("It worked!");
});
