var nodegit = require("../");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));
var path = "/tmp/nodegit-clone-demo";

fse.remove(path).then(function() {
  var entry;

  nodegit.Clone(
    "https://github.com/nodegit/nodegit.git",
    path,
    {
      remoteCallbacks: {
        certificateCheck: function() {
          // github will fail cert check on some OSX machines
          // this overrides that check
          return 1;
        }
      }
    })
  .then(function(repo) {
    return repo.getCommit("59b20b8d5c6ff8d09518454d4dd8b7b30f095ab5");
  })
  .then(function(commit) {
    return commit.getEntry("README.md");
  })
  .then(function(entryResult) {
    entry = entryResult;
    return entry.getBlob();
  })
  .done(function(blob) {
    console.log(entry.filename(), entry.sha(), blob.rawsize() + "b");
    console.log("========================================================\n\n");
    var firstTenLines = blob.toString().split("\n").slice(0, 10).join("\n");
    console.log(firstTenLines);
    console.log("...");
  });
});
