var promisify = require("promisify-node");
var fse = promisify("fs-extra");
var path = require("path");
var local = path.join.bind(path, __dirname);

// Have to wrap exec, since it has a weird callback signature.
var exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});

before(function() {
  this.timeout(350000);

  var url = "https://github.com/nodegit/test";
  fse.removeSync(local("repos"));

  return fse.mkdir(local("repos"))
    .then(function() {
      return exec("git init " + local("repos", "empty"));
    })
    .then(function() {
      return exec("git clone " + url + " " + local("repos", "workdir"));
    }).then(function() {
      return exec("git checkout rev-walk", {cwd: local("repos", "workdir")})
    }).then(function() {
      return exec("git checkout master", {cwd: local("repos", "workdir")})
    }).then(function() {
      return fse.mkdir(local("repos", "nonrepo"));
    }).then(function() {
      return fse.writeFile(local("repos", "nonrepo", "file.txt"),
        "This is a bogus file");
    });
});
