var promisify = require("thenify-all");
var fse = promisify(require("fs-extra"), ['mkdir', "remove", "writeFile"]);
var path = require("path");
var local = path.join.bind(path, __dirname);
var exec = promisify(require("child_process").exec);

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
