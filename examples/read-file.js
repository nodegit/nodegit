var nodegit = require("../"),
    path = require("path");

// This example opens a certain file, `README.md`, at a particular commit,
// and prints the first 10 lines as well as some metadata.
var _entry;
nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  .then(function(repo) {
    return repo.getCommit("59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5");
  })
  .then(function(commit) {
    return commit.getEntry("README.md");
  })
  .then(function(entry) {
    _entry = entry;
    return _entry.getBlob();
  })
  .then(function(blob) {
    console.log(_entry.name(), _entry.sha(), blob.rawsize() + "b");
    console.log("========================================================\n\n");
    var firstTenLines = blob.toString().split("\n").slice(0, 10).join("\n");
    console.log(firstTenLines);
    console.log("...");
  })
  .done();
