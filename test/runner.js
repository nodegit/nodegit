var fse = require("fs-extra");
var path = require("path");
var local = path.join.bind(path, __dirname);
var exec = require('../utils/execPromise');

var NodeGit = require('..');

var workdirPath = local("repos/workdir");
var constWorkdirPath = local("repos/constworkdir");

before(function() {
  this.timeout(350000);

  var url = "https://github.com/nodegit/test";
  return fse.remove(local("repos"))
    .then(function() {
      fse.remove(local("home"))
    })
    .then(function() {
      fse.mkdir(local("repos"));
    })
    .then(function() {
      return exec("git init " + local("repos", "empty"));
    })
    .then(function() {
      return exec("git clone " + url + " " + constWorkdirPath);
    })
    .then(function() {
      return exec("git clone " + url + " " + workdirPath);
    })
    .then(function() {
      return exec("git checkout rev-walk", {cwd: workdirPath});
    })
    .then(function() {
      return exec("git checkout checkout-test", {cwd: workdirPath});
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
    })
    .then(function() {
      return fse.mkdir(local("home"));
    })
    .then(function() {
      return fse.writeFile(local("home", ".gitconfig"),
        "[user]\n  name = John Doe\n  email = johndoe@example.com");
    });
});

beforeEach(function() {
  this.timeout(4000);
  return exec("git clean -xdf", {cwd: workdirPath})
  .then(function() {
    return exec("git checkout master", {cwd: workdirPath});
  })
  .then(function() {
    return exec("git reset --hard", {cwd: workdirPath});
  });
});

afterEach(function(done) {
  process.nextTick(function() {
    if (global.gc) {
      global.gc();
    }
    done();
  });
});
