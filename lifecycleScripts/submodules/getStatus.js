var path = require("path");
var rootDir = path.join(__dirname, "../..");
var exec = require(path.join(rootDir, "./utils/execPromise"));

module.exports = function getStatus() {
  return exec("git submodule status", { cwd: rootDir})
    .then(function(stdout) {
      if (!stdout) {
        // In the case where we pull from npm they pre-init the submodules for
        // us and `git submodule status` returns empty-string. In that case
        // we'll just assume that we're good.
        return Promise.resolve([]);
      }

      function getStatusPromiseFromLine(line) {
        var lineSections = line.trim().split(" ");
        var onNewCommit = !!~lineSections[0].indexOf("+");
        var needsInitialization = !!~lineSections[0].indexOf("-");
        var commitOid = lineSections[0].replace("+", "").replace("-", "");
        var name = lineSections[1];

        return exec("git status", { cwd: path.join(rootDir, name)})
          .then(function(workDirStatus) {
            return {
              commitOid: commitOid,
              onNewCommit: onNewCommit,
              name: name,
              needsInitialization: needsInitialization,
              workDirDirty: !~workDirStatus
                .trim()
                .split("\n")
                .pop()
                .indexOf("nothing to commit")
            };
          });
      }

      return Promise.all(stdout
        .trim()
        .split("\n")
        .map(getStatusPromiseFromLine)
      );
    })
    .catch(function() {
      // In the case that NodeGit is required from another project via npm we
      // won't be able to run submodule commands but that's ok since the
      // correct version of libgit2 is published with nodegit.
      return Promise.resolve([]);
    });
};
