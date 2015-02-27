var nodegit = require("../");
var path = require("path");
var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));

nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  .then(function(repo) {
    // Create a new branch on head
    return repo.getHeadCommit()
    .then(function(commit) {
      return repo.createBranch(
        "new-branch",
        commit,
        0,
        repo.defaultSignature(),
        "Created new-branch on HEAD");
    })
  }).done(function() {
    console.log("All done!");
  });
