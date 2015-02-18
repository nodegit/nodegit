var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));

describe("Index", function() {
  var Repository = require(local("../../lib/repository"));

  var reposPath = local("../repos/workdir/.git");

  before(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repo) {
        test.repo = repo;
        return repo.openIndex();
      })
      .then(function(index) {
        test.index = index;
      });
  });

  after(function() {
    this.index.clear();
  });

  it("can get the index of a repo and examine entries", function() {
    var entries = this.index.entries();

    assert.equal(entries[0].path(), ".gitignore");
  });

  it.only("can add all entries to the index", function() {
    var repo = this.repo;
    var index = this.index;
    var fileContent = {
      newFile1: "this has some content",
      newFile2: "and this will have more content"
    };
    var fileNames = Object.keys(fileContent);

    return Promise.all(fileNames.map(function(fileName) {
      fse.writeFile(path.join(repo.workdir(), fileName), fileContent[fileName]);
    }))
    .then(function() {
      return index.addAll();
    })
    .then(function() {
      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path());
      });

      assert.equal(newFiles.length, 2);
    });
  });
});
