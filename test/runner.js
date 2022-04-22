var fse = require("fs-extra");
var path = require("path");
var local = path.join.bind(path, __dirname);
var exec = require('../utils/execPromise');

var NodeGit = require('..');

var workdirPath = local("repos/workdir");
var constWorkdirPath = local("repos/constworkdir");

const testRepos = [
  "repos/bare",
  "repos/blameRepo",
  "repos/cherrypick",
  "repos/clone",
  "repos/constworkdir",
  "repos/convenientLineTest",
  "repos/empty",
  "repos/index",
  "repos/index",
  "repos/merge",
  "repos/merge-head",
  "repos/new",
  "repos/newrepo",
  "repos/nonrepo",
  "repos/rebase",
  "repos/renamedFileRepo",
  "repos/revertRepo",
  "repos/stagingRepo",
  "repos/submodule",
  "repos/submodule/nodegittest/",
  "repos/tree/",
  "repos/workdir",
];

before(function() {
  this.timeout(350000);

  var testUrl = "https://github.com/nodegit/test";
  var constTestUrl = "https://github.com/nodegit/test-frozen";
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
      return exec("git clone " + constTestUrl + " " + constWorkdirPath);
    })
    .then(function() {
      return exec("git clone " + testUrl + " " + workdirPath);
    })
    .then(function() {
      //to checkout the longpaths-checkout branch
      if(process.platform === "win32") {
        return exec("git config core.longpaths true", {cwd: workdirPath});
      }
      return Promise.resolve();
    })
    .then(function() {
      return exec("git checkout rev-walk", {cwd: workdirPath});
    })
    .then(function() {
      return exec("git checkout checkout-test", {cwd: workdirPath});
    })
    .then(function() {
      return exec("git checkout longpaths-checkout", {cwd: workdirPath});
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
    })
    .then( async function() {
      //mark all test repos as safe
      for(let repo of testRepos) {
        await exec(`git config --global --add safe.directory ${local(repo)}`);
      }
    })
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
