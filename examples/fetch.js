var nodegit = require("../");
var path = require("path");

nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  .then(function(repo) {
    return repo.fetch("origin", {
      credentials: function(url, userName) {
        return nodegit.Cred.sshKeyFromAgent(userName);
      }
    });
  }).then(function() {
    console.log("It worked!");
  });
