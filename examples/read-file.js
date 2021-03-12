const nodegit = require("../");
const path = require("path");

// This example opens a certain file, `README.md`, at a particular commit,
// and prints the first 10 lines as well as some metadata.

(async () => {
  const repo = await nodegit.Repository.open(path.resolve(__dirname, "../.git"));
  const commit = await repo.getCommit("59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5");
  const entry = await commit.getEntry("README.md");
  const blob = await entry.getBlob();
  
  console.log(entry.name(), entry.sha(), blob.rawsize() + "b");
  console.log("========================================================\n\n");
  const firstTenLines = blob.toString().split("\n").slice(0, 10).join("\n");
  console.log(firstTenLines);
  console.log("...");
})();

