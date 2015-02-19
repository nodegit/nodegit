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

  it("can add all entries to the index", function() {
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
    })
    .then(function() {
      return Promise.all(fileNames.map(function(fileName) {
        fse.remove(path.join(repo.workdir(), fileName));
      }));
    })
    .then(function() {
      index.clear();
    });
  });

  it("can remove entries from the index", function() {
    var repo = this.repo;
    var index = this.index;
    var fileContent = {
      newFile1: "this has some content",
      newFile2: "and this will have more content",
      differentFileName: "this has a different name and shouldn't be deleted"
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

      assert.equal(newFiles.length, 3);

      return index.removeAll("newFile*");
    })
    .then(function() {
      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path());
      });

      assert.equal(newFiles.length, 1);
    })
    .then(function() {
      return Promise.all(fileNames.map(function(fileName) {
        fse.remove(path.join(repo.workdir(), fileName));
      }));
    })
    .then(function() {
      index.clear();
    });
  });

  it("can update entries in the index", function() {
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

      return fse.remove(path.join(repo.workdir(), fileNames[0]));
    })
    .then(function() {
      return index.updateAll("newFile*");
    })
    .then(function() {
      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path());
      });

      assert.equal(newFiles.length, 1);
      return fse.remove(path.join(repo.workdir(), fileNames[1]));
    });
  });
});
