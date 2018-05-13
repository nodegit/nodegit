var nodegit = require("../");
var path = require("path");

var remoteBranchName = "REMOTE-BRANCH-NAME";

nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  .then(function(repo) {

    return repo.getHeadCommit()
      .then(function (targetCommit) {
        return repo.createBranch(remoteBranchName, targetCommit, false);
      })
      .then(function (reference) {
        return repo.checkoutBranch(reference, {});
      })
      .then(function () {
        return repo.getReferenceCommit(
          "refs/remotes/origin/" + remoteBranchName);
      })
      .then(function (commit) {
        nodegit.Reset.reset(repo, commit, 3, {});
      });

  }).done(function() {
  console.log("All done!");
});
