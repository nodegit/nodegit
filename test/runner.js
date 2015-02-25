var promisify = require("promisify-node");
var fse = promisify("fs-extra");
var path = require("path");
var local = path.join.bind(path, __dirname);

// Have to wrap exec, since it has a weird callback signature.
var exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});

var workdirPath = local("repos/workdir");

before(function() {
  this.timeout(350000);

  var url = "https://github.com/nodegit/test";
  return fse.remove(local("repos"))
    .then(function() {
      fse.mkdir(local("repos"));
    })
    .then(function() {
      return exec("git init " + local("repos", "empty"));
    })
    .then(function() {
      return exec("git clone " + url + " " + workdirPath);
    })
    .then(function() {
      return exec("git checkout rev-walk", {cwd: workdirPath});
    })
    .then(function() {
      return exec("git checkout master", {cwd: workdirPath});
    })
    .then(function() {
      return fse.mkdir(local("repos", "nonrepo"));
    })
    .then(function() {
      return fse.writeFile(local("repos", "nonrepo", "file.txt"),
        "This is a bogus file");
    });
});

beforeEach(function() {
  return exec("git clean -xdf", {cwd: workdirPath})
    .then(function() {
      return exec("git reset --hard", {cwd: workdirPath});
    });
});

afterEach(function() {
  if (this.repository) {
    this.repository.free();
    delete this.repository;
  }

  else if (this.repo) {
    this.repo.free();
    delete this.repo;
  }
});
