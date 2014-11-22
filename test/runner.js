var promisify = require("promisify-node");
var fse = promisify("fs-extra");

// Have to wrap exec, since it has a weird callback signature.
var exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});

before(function(done) {
  this.timeout(350000);

  var url = "https://github.com/nodegit/test";
  var done = done.bind(null, null);

  function initEmpty() {
    return exec("git init test/repos/empty");
  }

  fse.removeSync("test/repos")

  fse.mkdir("test/repos").then(initEmpty, initEmpty)
    .then(function() {
      return exec("git clone " + url + " test/repos/workdir");
    }).then(function() {
      return exec("git checkout rev-walk", {cwd: "test/repos/workdir"})
    }).then(function() {
      return exec("git checkout master", {cwd: "test/repos/workdir"})
    }).then(function() {
      var nonrepo = "test/repos/nonrepo";

      function writeBogus() {
        return fse.writeFile(nonrepo + "/file.txt", "This is a bogus file");
      }

      return fse.mkdir(nonrepo).then(writeBogus, writeBogus);
    }).then(done, done);
});
