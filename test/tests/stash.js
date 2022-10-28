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
    const fileName = "README.md";
    const fileContent = "Cha-cha-cha-chaaaaaangessssss";
    const filePath = path.join(repo.workdir(), fileName);
    let oldContent;
    let stashes = [];
    let stashOid;

    return fse.readFile(filePath)
      .then((content) => {
        oldContent = content;
        return fse.writeFile(filePath, fileContent);
      })
      .then(() => repo.defaultSignature())
      .then((signature) => {
        return Stash.save(repo, signature, stashMessage, 0);
      })
      .then((oid) => {
        stashOid = oid;
        var stashCb = function(index, message, oid) {
          stashes.push({index: index, message: message, oid: oid});
        };

        return Stash.foreach(repo, stashCb);
      })
      .then(() => {
        assert.equal(stashes.length, 1);
        assert.equal(stashes[0].index, 0);
        const expectedMessage = !stashMessage ?
          "WIP on master: 32789a7 Fixes EJS not being installed via NPM" :
          "On master: " + stashMessage;
        assert.equal(stashes[0].message, expectedMessage);
        assert.equal(stashes[0].oid.toString(), stashOid.toString());

        return Stash.drop(repo, 0);
      })
      .then(() => {
        stashes = [];
        var stashCb = (index, message, oid) => {
          stashes.push({index: index, message: message, oid: oid});
        };

        return Stash.foreach(repo, stashCb);
      })
      .then(() => {
        assert.equal(stashes.length, 0);
      })
      .catch((e) => {
        return fse.writeFile(filePath, oldContent)
          .then(() => {
            return Promise.reject(e);
          });
      });
  }

  it("can save and drop a stash", function() {
    return saveDropStash(this.repository, "stash test");
  });

  it("can save a stash with no message and drop it", function() {
    return saveDropStash(this.repository, null);
  });

  it("can save and pop a stash", function() {
    const fileNameA = "README.md";
    const fileNameB = "install.js";
    let oldContentA;
    let oldContentB;
    const fileContent = "Cha-cha-cha-chaaaaaangessssss";
    const repo = this.repository;
    const filePathA = path.join(repo.workdir(), fileNameA);
    const filePathB = path.join(repo.workdir(), fileNameB);
    const stashMessage = "stash test";

    return fse.readFile(filePathA, "utf-8")
      .then((content) => {
        oldContentA = content;
        return fse.writeFile(filePathA, fileContent);
      })
      .then(() => {
        return fse.readFile(filePathB, "utf-8");
      })
      .then((content) => {
        oldContentB = content;
        return fse.writeFile(filePathB, fileContent);
      })
      .then(() => repo.defaultSignature())
      .then((signature) => {
        return Stash.save(repo, signature, stashMessage, 0);
      })
      .then(() => {
        return fse.readFile(filePathA, "utf-8");
      })
      .then((content) => {
        assert.equal(oldContentA, content);
        return fse.readFile(filePathB, "utf-8");
      })
      .then((content) => {
        assert.equal(oldContentB, content);
        return Stash.pop(repo, 0);
      })
      .then(() => {
        return fse.readFile(filePathA, "utf-8");
      })
      .then((content) => {
        assert.equal(fileContent, content);
        return fse.readFile(filePathB, "utf-8");
      })
      .then((content) => {
        assert.equal(fileContent, content);
      });
  });

  it("can save a stash, change files, and fail to pop stash", function() {
      const fileName = "README.md";
      const fileContent = "Cha-cha-cha-chaaaaaangessssss";
      const fileContent2 = "Somewhere over the repo, changes were made.";
      const repo = this.repository;
      const filePath = path.join(repo.workdir(), fileName);
      let oldContent;
      const stashMessage = "stash test";

      return fse.readFile(filePath)
        .then((content) => {
          oldContent = content;
          return fse.writeFile(filePath, fileContent);
        })
        .then(() => repo.defaultSignature())
        .then((signature) => {
          return Stash.save(repo, signature, stashMessage, 0);
        })
        .then(() => {
          return fse.writeFile(filePath, fileContent2);
        })
        .then(() => {
          return Stash.pop(repo, 0);
        })
        .catch((reason) => {
          if (reason.message !== "1 conflict prevents checkout") {
            throw reason;
          } else {
            return Promise.resolve();
          }
        });
  });

  it("can save, apply, then drop the stash", function() {
      const fileName = "README.md";
      const fileContent = "Cha-cha-cha-chaaaaaangessssss";
      const repo = this.repository;
      const filePath = path.join(repo.workdir(), fileName);
      let oldContent;
      const stashMessage = "stash test";

      return fse.readFile(filePath)
        .then((content) => {
          oldContent = content;
          return fse.writeFile(filePath, fileContent);
        })
        .then(() => repo.defaultSignature())
        .then((signature) => {
          return Stash.save(repo, signature, stashMessage, 0);
        })
        .then(() => {
          return Stash.apply(repo, 0);
        })
        .then(() => {
          return Stash.drop(repo, 0);
        }, () => {
          throw new Error("Unable to drop stash after apply.");
        })
        .then(() => {
          return Stash.drop(repo, 0);
        })
        .catch((reason) => {
          if (reason.message !== "reference 'refs/stash' not found") {
            throw reason;
          }
        });
  });

  it("can save multiple stashes and pop an arbitrary stash", function() {
    const fileName = "README.md";
    const fileContentA = "Hi. It's me. I'm the dog. My name is the dog.";
    const fileContentB = "Everyone likes me. I'm cute.";
    const fileContentC =
      "I think I will bark at nothing now. Ba. Ba. Baba Baba.";
    const repo = this.repository;
    const filePath = path.join(repo.workdir(), fileName);
    let oldContent;
    const stashMessageA = "stash test A";
    const stashMessageB = "stash test B";
    const stashMessageC = "stash test C";

    const writeAndStash = (path, content, message) => {
      return fse.writeFile(path, content)
        .then(() => repo.defaultSignature())
        .then((signature) => {
          return Stash.save(repo, signature, message, 0);
        });
    };

    return fse.readFile(filePath, "utf-8")
      .then((content) => {
        oldContent = content;
        return writeAndStash(filePath, fileContentA, stashMessageA);
      })
      .then(() => {
        return writeAndStash(filePath, fileContentB, stashMessageB);
      })
      .then(() => {
        return writeAndStash(filePath, fileContentC, stashMessageC);
      })
      .then(() => {
        return fse.readFile(filePath, "utf-8");
      })
      .then((content) => {
        assert.equal(oldContent, content);
        return Stash.pop(repo, 1);
      })
      .then(() => {
        return fse.readFile(filePath, "utf-8");
      })
      .then((content) => {
        assert.equal(fileContentB, content);
      });
  });

  it("can partial stash the workdir and pop it", function() {
    const repo = this.repository;
    
    const fileName1 = "README.md";
    const fileName2 = "install.js";
    const fileName3 = "LICENSE";

    const fileContentA = "Hi. It's me. I'm the dog. My name is the dog.";
    const fileContentB = "Everyone likes me. I'm cute.";

    let oldContentA;
    let oldContentB;
    let oldContentC;

    const filePath1 = path.join(repo.workdir(), fileName1);
    const filePath2 = path.join(repo.workdir(), fileName2);
    const filePath3 = path.join(repo.workdir(), fileName3);

    const options = {
      flags: 0,
      message: "stast test",
      paths: [fileName1, fileName2]
    };

    return fse.readFile(filePath1, "utf-8")
      .then((content) => {
        oldContentA = content;
        return fse.readFile(filePath2, "utf-8");
      })
      .then((content) => {
        oldContentB = content;
        return fse.readFile(filePath3, "utf-8");
      })
      .then((content) => {
        oldContentC = content;
        return fse.writeFile(filePath1, fileContentA);
      })
      .then(() => fse.writeFile(filePath2, fileContentB))
      .then(() => repo.defaultSignature())
      .then((signature) => {
        options.stasher = signature;
        return Stash.saveWithOpts(repo, options);
      })
      .then(() => fse.readFile(filePath1, "utf-8"))
      .then((content) => {
        assert.equal(oldContentA, content);
        return fse.readFile(filePath2, "utf-8");
      })
      .then((content) => {
        assert.equal(oldContentB, content);
        return fse.readFile(filePath3, "utf-8");
      })
      .then((content) => {
        assert.equal(oldContentC, content);
        return Stash.pop(repo, 0);
      })
      .then(() => fse.readFile(filePath1, "utf-8"))
      .then((content) => {
        assert.equal(fileContentA, content);
        return fse.readFile(filePath2, "utf-8");
      })
      .then((content) => {
        assert.equal(fileContentB, content);
        return fse.readFile(filePath3, "utf-8");
      })
      .then((content) => {
        assert.equal(oldContentC, content);
      });
  });
});
