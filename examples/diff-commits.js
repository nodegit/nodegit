var nodegit = require("../");
var path = require("path");

// This code examines the diffs between a particular commit and all of its
// parents. Since this commit is not a merge, it only has one parent. This is
// similar to doing `git show`.

nodegit.Repository.open(path.resolve(__dirname, "../.git"))
.then(function(repo) {
  return repo.getCommit("59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5");
})
.then(function(commit) {
  console.log("commit " + commit.sha());
  console.log("Author:", commit.author().name() +
    " <" + commit.author().email() + ">");
  console.log("Date:", commit.date());
  console.log("\n    " + commit.message());

  return commit.getDiff();
})
.done(function(diffList) {
  diffList.forEach(function(diff) {
    diff.patches().then(function(patches) {
      patches.forEach(function(patch) {
        patch.hunks().then(function(hunks) {
          hunks.forEach(function(hunk) {
            hunk.lines().then(function(lines) {
              console.log("diff", patch.oldFile().path(),
                patch.newFile().path());
              console.log(hunk.header().trim());
              lines.forEach(function(line) {
                console.log(String.fromCharCode(line.origin()) +
                  line.content().trim());
              });
            });
          });
        });
      });
    });
  });
});
