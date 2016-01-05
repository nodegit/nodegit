var nodegit = require("../"),
    path = require("path");

// This code walks the history of the master branch and prints results
// that look very similar to calling `git log` from the command line

nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  .then(function(repo) {
    return repo.getMasterCommit();
  })
  .then(function(firstCommitOnMaster){
    // History returns an event.
    var history = firstCommitOnMaster.history(nodegit.Revwalk.SORT.Time);
    var commits = [];

    // History emits "commit" event for each commit in the branch's history
    history.on("commit", function(commit) {
      return commit.getDiff()
      .then(function(diffList) {
        diffList.map(function(diff) {
          diff.patches().then(function(patches) {
            patches.map(function(patch) {
              var result =
                !!~patch.oldFile().path().indexOf("descriptor.json") ||
                !!~patch.newFile().path().indexOf("descriptor.json");

              if(result && !~commits.indexOf(commit)) {
                commits.push(commit);
              }
            });
          });
        });
      });
    });

    history.on("end", function() {
      commits.forEach(function(commit) {
        console.log("commit " + commit.sha());
        console.log("Author:", commit.author().name() +
          " <" + commit.author().email() + ">");
        console.log("Date:", commit.date());
        console.log("\n    " + commit.message());
      });
    });

    // Don't forget to call `start()`!
    history.start();
  })
  .done();
