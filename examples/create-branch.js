var nodegit = require("../");
var path = require("path");

nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  .then(function(repo) {
    // Create a new branch on head
    return repo.getHeadCommit()
    .then(function(commit) {
      return repo.createBranch(
        "new-branch",
        commit,
        0);
    });
  }).done(function() {
    console.log("All done!");
  });
