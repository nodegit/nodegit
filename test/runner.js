var promisify = require("promisify-node");
var fs = promisify("fs");
var exec = promisify("child_process").exec; 

before(function(done) {
  var url = "https://github.com/nodegit/nodegit";

  fs.exists("test/repos").then(null, function() {
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
  }).then(done.bind(null, undefined), done.bind(null, undefined));
});
