var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));

var writeFile = promisify(function(filename, data, callback) {
  return require("fs").writeFile(filename, data, {}, callback);
});

describe("Index", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repo) {
        test.repository = repo;
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

    assert.equal(entries[0].path, ".gitignore");
  });

  it("can add all entries to the index", function() {
    var repo = this.repository;
    var index = this.index;
    var fileContent = {
      newFile1: "this has some content",
      newFile2: "and this will have more content"
    };
    var fileNames = Object.keys(fileContent);

    return Promise.all(fileNames.map(function(fileName) {
      return writeFile(
        path.join(repo.workdir(), fileName),
        fileContent[fileName]);
    }))
    .then(function() {
      return index.addAll();
    })
    .then(function() {
      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path);
      });

      assert.equal(newFiles.length, 2);
    })
    .then(function() {
      return Promise.all(fileNames.map(function(fileName) {
        return fse.remove(path.join(repo.workdir(), fileName));
      }));
    })
    .then(function() {
      index.clear();
    });
  });

  it("can remove entries from the index", function() {
    var repo = this.repository;
    var index = this.index;
    var fileContent = {
      newFile1: "this has some content",
      newFile2: "and this will have more content",
      differentFileName: "this has a different name and shouldn't be deleted"
    };
    var fileNames = Object.keys(fileContent);

    return Promise.all(fileNames.map(function(fileName) {
      return writeFile(
        path.join(repo.workdir(), fileName),
        fileContent[fileName]);
    }))
    .then(function() {
      return index.addAll();
    })
    .then(function() {
      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path);
      });

      assert.equal(newFiles.length, 3);

      return index.removeAll("newFile*");
    })
    .then(function() {
      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path);
      });

      assert.equal(newFiles.length, 1);
    })
    .then(function() {
      return Promise.all(fileNames.map(function(fileName) {
        return fse.remove(path.join(repo.workdir(), fileName));
      }));
    })
    .then(function() {
      index.clear();
    });
  });

  it("can update entries in the index", function() {
    var repo = this.repository;
    var index = this.index;
    var fileContent = {
      newFile1: "this has some content",
      newFile2: "and this will have more content"
    };
    var fileNames = Object.keys(fileContent);

    return Promise.all(fileNames.map(function(fileName) {
      return writeFile(
        path.join(repo.workdir(), fileName),
        fileContent[fileName]);
    }))
    .then(function() {
      return index.addAll();
    })
    .then(function() {
      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path);
      });

      assert.equal(newFiles.length, 2);

      return fse.remove(path.join(repo.workdir(), fileNames[0]));
    })
    .then(function() {
      return index.updateAll("newFile*");
    })
    .then(function() {
      var newFiles = index.entries().filter(function(entry) {
        return ~fileNames.indexOf(entry.path);
      });

      assert.equal(newFiles.length, 1);
      return fse.remove(path.join(repo.workdir(), fileNames[1]));
    });
  });
});
