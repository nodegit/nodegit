var assert = require("assert");
var path = require("path");
var promisify = require("promisify-node");
var Promise = require("nodegit-promise");
var fse = promisify(require("fs-extra"));
var local = path.join.bind(path, __dirname);

describe("Repository", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Index = NodeGit.Index;
  var Signature = NodeGit.Signature;

  var reposPath = local("../repos/workdir");
  var newRepo = local("../repos/newrepo");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      });
  });

  it("can open a valid repository", function() {
    assert.ok(this.repository instanceof Repository);
  });

  it("cannot open an invalid repository", function() {
    return Repository.open("repos/nonrepo")
      .then(null, function(err) {
        assert.ok(err instanceof Error);
      });
  });

  it("does not try to open paths that don't exist", function() {
    var missingPath = "/surely/this/directory/does/not/exist/on/this/machine";

    return Repository.open(missingPath)
      .then(null, function(err) {
        assert.ok(err instanceof Error);
      });
  });

  it("can initialize a repository into a folder", function() {
    return Repository.init(newRepo, 1)
      .then(function(path, isBare) {
        return Repository.open(newRepo);
      });
  });

  it("can utilize repository init options", function() {
    return fse.remove(newRepo)
      .then(function() {
        return Repository.initExt(newRepo, {
          flags: Repository.INIT_FLAG.MKPATH
        });
      });
  });

  it("can read the index", function() {
    return this.repository.index()
      .then(function(index) {
        assert.ok(index instanceof Index);
      });
  });

  it("can list remotes", function() {
    return this.repository.getRemotes()
      .then(function(remotes) {
        assert.equal(remotes.length, 1);
        assert.equal(remotes[0], "origin");
      });
  });

  it("can get the current branch", function() {
    return this.repository.getCurrentBranch()
      .then(function(branch) {
        assert.equal(branch.shorthand(), "master");
      });
  });

  it("can get the default signature", function() {
    var sig = this.repository.defaultSignature();

    assert(sig instanceof Signature);
  });

  it("gets statuses with StatusFile", function() {
    var fileName = "my-new-file-that-shouldnt-exist.file";
    var fileContent = "new file from repository test";
    var repo = this.repository;
    var filePath = path.join(repo.workdir(), fileName);

    return fse.writeFile(filePath, fileContent)
      .then(function() {
        return repo.getStatus().then(function(statuses) {
          assert.equal(statuses.length, 1);
          assert.equal(statuses[0].path(), fileName);
          assert.ok(statuses[0].isNew());
        });
      })
      .then(function() {
        return fse.remove(filePath);
      })
      .catch(function (e) {
        return fse.remove(filePath)
          .then(function() {
            return Promise.reject(e);
          });
      });
  });

  it("gets extended statuses", function() {
    var fileName = "my-new-file-that-shouldnt-exist.file";
    var fileContent = "new file from repository test";
    var repo = this.repository;
    var filePath = path.join(repo.workdir(), fileName);

    return fse.writeFile(filePath, fileContent)
      .then(function() {
        var statuses = repo.getStatusExt();

        assert.equal(statuses.length, 1);
        assert.equal(statuses[0].path(), fileName);
        assert.equal(statuses[0].indexToWorkdir().newFile().path(), fileName);
        assert.ok(statuses[0].isNew());
      })
      .then(function() {
        return fse.remove(filePath);
      })
      .catch(function (e) {
        return fse.remove(filePath)
          .then(function() {
            return Promise.reject(e);
          });
      });
  });

  it("gets fetch-heads", function() {
    var repo = this.repository;
    var foundMaster;

    return repo.fetch("origin", {
      credentials: function(url, userName) {
        return NodeGit.Cred.sshKeyFromAgent(userName);
      },
      certificateCheck: function() {
        return 1;
      }
    })
    .then(function() {
      return repo.fetchheadForeach(function(refname, remoteUrl, oid, isMerge) {
        if (refname == "refs/heads/master") {
          foundMaster = true;
          assert.equal(refname, "refs/heads/master");
          assert.equal(remoteUrl, "https://github.com/nodegit/test");
          assert.equal(
            oid.toString(),
            "32789a79e71fbc9e04d3eff7425e1771eb595150");
          assert.equal(isMerge, 1);
        }
      });
    })
    .then(function() {
      if (!foundMaster) {
        throw new Error("Couldn't find master in iteration of fetch heads");
      }
    });
  });
});
