const nodegit = require("../");
const path = require("path");

// This code examines the diffs between a particular commit and all of its
// parents. Since this commit is not a merge, it only has one parent. This is
// similar to doing `git show`.

(async () => {
  const repo = await nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  const commit = await repo.getCommit("59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5");
  console.log("commit " + commit.sha());
  console.log(
    "Author:", commit.author().name() +
    " <" + commit.author().email() + ">"
  );
  console.log("Date:", commit.date());
  console.log("\n    " + commit.message());

  const diffList = await commit.getDiff();
  for (const diff of diffList) {
    const patches = await diff.patches();
    for (const patch of patches) {
      const hunks = await patch.hunks();
      for (const hunk of hunks) {
        const lines = await hunk.lines();
        console.log(
          "diff",
          patch.oldFile().path(),
          patch.newFile().path()
        );
        console.log(hunk.header().trim());
        for (const line of lines) {
          console.log(
            String.fromCharCode(line.origin()) +
            line.content().trim()
          );
        }
      }
    }
  }
})();
