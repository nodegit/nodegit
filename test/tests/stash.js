var assert = require("assert");
var path = require("path");
var fse = require("fs-extra");
var local = path.join.bind(path, __dirname);

describe("Stash", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Stash = NodeGit.Stash;

  var reposPath = local("../repos/workdir");

  before(function() {
    var test = this;
    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      });
  });

  it("gets no stashes on clean working directory", function() {
    var stashes = [];
    var stashCb = function(index, message, oid) {
      stashes.push({index: index, message: message, oid: oid});
    };

    return Stash.foreach(this.repository, stashCb)
      .then(function() {
        assert.equal(stashes.length, 0);
      });
  });

  function saveDropStash(repo, stashMessage) {
    var fileName = "README.md";
    var fileContent = "Cha-cha-cha-chaaaaaangessssss";
    var filePath = path.join(repo.workdir(), fileName);
    var oldContent;
    var stashes = [];
    var stashOid;

    return fse.readFile(filePath)
      .then(function(content) {
        oldContent = content;
        return fse.writeFile(filePath, fileContent);
      })
      .then(function() {
        return Stash.save(repo, repo.defaultSignature(), stashMessage, 0);
      })
      .then(function(oid) {
        stashOid = oid;
        var stashCb = function(index, message, oid) {
          stashes.push({index: index, message: message, oid: oid});
        };

        return Stash.foreach(repo, stashCb);
      })
      .then(function() {
        assert.equal(stashes.length, 1);
        assert.equal(stashes[0].index, 0);
        assert.equal(stashes[0].message, "On master: " + stashMessage);
        assert.equal(stashes[0].oid.toString(), stashOid.toString());

        return Stash.drop(repo, 0);
      })
      .then(function () {
        stashes = [];
        var stashCb = function(index, message, oid) {
          stashes.push({index: index, message: message, oid: oid});
        };

        return Stash.foreach(repo, stashCb);
      })
      .then(function() {
        assert.equal(stashes.length, 0);
      })
      .catch(function(e) {
        return fse.writeFile(filePath, oldContent)
          .then(function() {
            return Promise.reject(e);
          });
      });
  }

  it("can save and drop a stash", function() {
    saveDropStash(this.repository, "stash test");
  });

  it("can save a stash with no message and drop it", function() {
    saveDropStash(this.repository, null);
  });

  it("can save and pop a stash", function() {
    var fileNameA = "README.md";
    var fileNameB = "install.js";
    var oldContentA;
    var oldContentB;
    var fileContent = "Cha-cha-cha-chaaaaaangessssss";
    var repo = this.repository;
    var filePathA = path.join(repo.workdir(), fileNameA);
    var filePathB = path.join(repo.workdir(), fileNameB);
    var stashMessage = "stash test";

    return fse.readFile(filePathA, "utf-8")
      .then(function(content) {
        oldContentA = content;
        return fse.writeFile(filePathA, fileContent);
      })
      .then(function() {
        return fse.readFile(filePathB, "utf-8");
      })
      .then(function(content) {
        oldContentB = content;
        return fse.writeFile(filePathB, fileContent);
      })
      .then(function() {
        return Stash.save(repo, repo.defaultSignature(), stashMessage, 0);
      })
      .then(function() {
        return fse.readFile(filePathA, "utf-8");
      })
      .then(function(content) {
        assert.equal(oldContentA, content);
        return fse.readFile(filePathB, "utf-8");
      })
      .then(function(content) {
        assert.equal(oldContentB, content);
        return Stash.pop(repo, 0);
      })
      .then(function() {
        return fse.readFile(filePathA, "utf-8");
      })
      .then(function(content) {
        assert.equal(fileContent, content);
        return fse.readFile(filePathB, "utf-8");
      })
      .then(function(content) {
        assert.equal(fileContent, content);
      });
  });

  it("can save a stash, change files, and fail to pop stash", function() {
      var fileName = "README.md";
      var fileContent = "Cha-cha-cha-chaaaaaangessssss";
      var fileContent2 = "Somewhere over the repo, changes were made.";
      var repo = this.repository;
      var filePath = path.join(repo.workdir(), fileName);
      var oldContent;
      var stashMessage = "stash test";

      return fse.readFile(filePath)
        .then(function(content) {
          oldContent = content;
          return fse.writeFile(filePath, fileContent);
        })
        .then(function() {
          return Stash.save(repo, repo.defaultSignature(), stashMessage, 0);
        })
        .then(function() {
          return fse.writeFile(filePath, fileContent2);
        })
        .then(function() {
          return Stash.pop(repo, 0);
        })
        .catch(function(reason) {
          if (reason.message !== "1 conflict prevents checkout") {
            throw reason;
          } else {
            return Promise.resolve();
          }
        });
  });

  it("can save, apply, then drop the stash", function() {
      var fileName = "README.md";
      var fileContent = "Cha-cha-cha-chaaaaaangessssss";
      var repo = this.repository;
      var filePath = path.join(repo.workdir(), fileName);
      var oldContent;
      var stashMessage = "stash test";

      return fse.readFile(filePath)
        .then(function(content) {
          oldContent = content;
          return fse.writeFile(filePath, fileContent);
        })
        .then(function() {
          return Stash.save(repo, repo.defaultSignature(), stashMessage, 0);
        })
        .then(function() {
          return Stash.apply(repo, 0);
        })
        .then(function() {
          return Stash.drop(repo, 0);
        }, function() {
          throw new Error("Unable to drop stash after apply.");
        })
        .then(function() {
          return Stash.drop(repo, 0);
        })
        .catch(function(reason) {
          if (reason.message !== "Reference 'refs/stash' not found") {
            Promise.reject();
          }
        });
  });

  it("can save multiple stashes and pop an arbitrary stash", function() {
    var fileName = "README.md";
    var fileContentA = "Hi. It's me. I'm the dog. My name is the dog.";
    var fileContentB = "Everyone likes me. I'm cute.";
    var fileContentC = "I think I will bark at nothing now. Ba. Ba. Baba Baba.";
    var repo = this.repository;
    var filePath = path.join(repo.workdir(), fileName);
    var oldContent;
    var stashMessageA = "stash test A";
    var stashMessageB = "stash test B";
    var stashMessageC = "stash test C";

    function writeAndStash(path, content, message) {
      return fse.writeFile(path, content)
        .then(function() {
          return Stash.save(repo, repo.defaultSignature(), message, 0);
        });
    }

    return fse.readFile(filePath, "utf-8")
      .then(function (content) {
        oldContent = content;
        return writeAndStash(filePath, fileContentA, stashMessageA);
      })
      .then(function() {
        return writeAndStash(filePath, fileContentB, stashMessageB);
      })
      .then(function() {
        return writeAndStash(filePath, fileContentC, stashMessageC);
      })
      .then(function() {
        return fse.readFile(filePath, "utf-8");
      })
      .then(function(content) {
        assert.equal(oldContent, content);
        return Stash.pop(repo, 1);
      })
      .then(function() {
        return fse.readFile(filePath, "utf-8");
      })
      .then(function(content) {
        assert.equal(fileContentB, content);
      });
  });
});
