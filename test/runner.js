var fse = require("fs-extra");
var path = require("path");
var local = path.join.bind(path, __dirname);
var exec = require('../utils/execPromise');

var NodeGit = require('..');

if(process.env.NODEGIT_TEST_THREADSAFETY) {
  console.log('Enabling thread safety in NodeGit');
  NodeGit.enableThreadSafety();
} else if (process.env.NODEGIT_TEST_THREADSAFETY_ASYNC) {
  console.log('Enabling thread safety for async actions only in NodeGit');
  NodeGit.setThreadSafetyStatus(NodeGit.THREAD_SAFETY.ENABLED_FOR_ASYNC_ONLY);
}

var workdirPath = local("repos/workdir");

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
