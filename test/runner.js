var promisify = require("promisify-node");
var fs = promisify("fs");

// Have to wrap exec, since it has a weird callback signature.
var exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});

before(function(done) {
  this.timeout(150000);

  var url = "https://github.com/nodegit/test";
  var done = done.bind(null, null);

  function initEmpty() {
    return exec("git init test/repos/empty");
  }

  fs.mkdir("test/repos").then(initEmpty, initEmpty)
    .then(function() {
      return exec("git clone " + url + " test/repos/workdir");
    }).then(function() {
      return exec("git checkout rev-walk", {cwd: "test/repos/workdir"})
    }).then(function() {
      return exec("git checkout master", {cwd: "test/repos/workdir"})
    }).then(function() {
      var nonrepo = "test/repos/nonrepo";

      function writeBogus() {
        return fs.writeFile(nonrepo + "/file.txt", "This is a bogus file");
      }

      return fs.mkdir(nonrepo).then(writeBogus, writeBogus);
    }).then(done, done);
});
