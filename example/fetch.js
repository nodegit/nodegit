var nodegit = require('../');
var path = require('path');

nodegit.Repository.open(path.resolve(__dirname, '../.git')).then(function(repo) {
  return repo.fetch("origin");
})
.done(function() {
  console.log("It worked!");
});
