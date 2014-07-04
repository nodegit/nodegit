var promisify = require("promisify-node");
var fs = promisify("fs");

// Have to wrap exec, since it has a weird callback signature.
var exec = promisify(function(command, callback) {
  return require("child_process").exec(command, callback);
});

before(function(done) {
  this.timeout(5000);

  var url = "https://github.com/nodegit/nodegit";
  var done = done.bind(null, null);

  fs.exists("test/repos").then(function() {
    return fs.mkdir("test/repos").then(function() {
      return exec("git init test/repos/empty");
    }).then(function() {
      return exec("git clone " + url + " test/repos/workdir");
    }).then(function() {
      var nonrepo = "test/repos/nonrepo";

      return fs.mkdir(nonrepo).then(function() {
        return fs.writeFile(nonrepo + "/file.txt", "This is a bogus file");
      });
    })
  }).then(done, done);
});
