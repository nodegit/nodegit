var promisify = require("promisify-node");
var fs = promisify("fs");
var exec = promisify(("child_process").exec);
var path = require("path");

before(function() {
  var url = "https://github.com/nodegit/nodegit";

  return fs.exists(".reposCache").then(function() {
    return fs.mkdir("repos").then(function() {
      return exec("git init repos/empty");
    }).then(function() {
      return exec("git clone " + url + " repos/workdir");
    }).then(function() {
      return fs.mkdir("repos/nonrepo").then(function() {
        return fs.writeFile("repos/nonrepo/file.txt", "This is a bogus file");
      });
    })
  });
});
