var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var fse = require("fs-extra");

describe("Cherrypick", function() {
  var RepoUtils = require("../utils/repository_setup");
  var NodeGit = require("../../");
  var Cherrypick = NodeGit.Cherrypick;

  var repoPath = local("../repos/cherrypick");

  beforeEach(function() {
    var test = this;
    return RepoUtils.createRepository(repoPath)
      .then(function(repo) {
        test.repository = repo;
      });
  });

  after(function() {
    return fse.remove(repoPath);
  });

  it("can cherrypick a commit onto the index", function() {
    var repo = this.repository;
    var workDirPath = repo.workdir();
    var repoInfo;

    return RepoUtils.setupBranches(repo, true)
      .then(function(info) {
        repoInfo = info;

        assert(!fse.existsSync(path.join(workDirPath, repoInfo.theirFileName)),
          repoInfo.theirFileName + " shouldn't exist");

        var promise = Cherrypick.cherrypick(repo, repoInfo.theirCommit, {});
        assert(promise.then);
        return promise;
      })
      .then(function() {
        assert(fse.existsSync(path.join(workDirPath, repoInfo.theirFileName)),
          repoInfo.theirFileName + " should exist");

        // Cherrypick.cherrypick leaves the repo in a cherrypick state
        assert.equal(repo.state(), NodeGit.Repository.STATE.CHERRYPICK);
        assert.ok(repo.isCherrypicking());

        // cleanup
        assert.equal(repo.stateCleanup(), 0);
        assert.equal(repo.state(), NodeGit.Repository.STATE.NONE);
        assert.ok(repo.isDefaultState());
      });
  });

  it("can cherrypick a commit onto another specified commit", function() {
    var repo = this.repository;
    var workDirPath = repo.workdir();
    var repoInfo;

    return RepoUtils.setupBranches(repo)
      .then(function(info) {
        repoInfo = info;

        assert(!fse.existsSync(path.join(workDirPath, repoInfo.ourFileName)),
          repoInfo.ourFileName + " shouldn't exist");
        assert(!fse.existsSync(path.join(workDirPath, repoInfo.theirFileName)),
          repoInfo.theirFileName + " shouldn't exist");

        var promise = Cherrypick.commit(repo, repoInfo.theirCommit,
          repoInfo.ourCommit, 0, {});
        assert(promise.then);
        return promise;
      })
      .then(function(index) {
        assert(index);
        return index.writeTreeTo(repo);
      })
      .then(function(oid) {
        return repo.getTree(oid);
      })
      .then(function(tree) {
        var opts = {
          checkoutStrategy: NodeGit.Checkout.STRATEGY.FORCE
        };

        return NodeGit.Checkout.tree(repo, tree, opts);
      })
      .then(function() {
        assert(fse.existsSync(path.join(workDirPath, repoInfo.ourFileName)),
          repoInfo.ourFileName + " should exist");
        assert(fse.existsSync(path.join(workDirPath, repoInfo.theirFileName)),
          repoInfo.theirFileName + " should exist");
      });
  });

  it("can cherrypick a stash to apply it", function() {
    var repo = this.repository;
    var workDirPath = repo.workdir();
    var repoInfo;
    var cherrypickOid;

    var addedContent = "\nIt makes things E-Z!";

    return RepoUtils.setupBranches(repo, true)
      .then(function(info) {
        repoInfo = info;

        return repo.getStatus();
      })
      .then(function(statuses) {
        assert.equal(statuses.length, 0);

        return fse.writeFile(path.join(workDirPath, repoInfo.ourFileName),
          repoInfo.ourFileContent + addedContent);
      })
      .then(function() {
        return repo.getStatus();
      })
      .then(function(statuses) {
        assert.equal(statuses.length, 1);

        return NodeGit.Stash.save(repo, repoInfo.ourSignature, "our stash", 0);
      })
      .then(function(oid) {
        cherrypickOid = oid;

        return fse.readFile(path.join(workDirPath, repoInfo.ourFileName));
      })
      .then(function(fileContent) {
        assert.equal(fileContent, repoInfo.ourFileContent);

        return repo.getStatus();
      })
      .then(function(statuses) {
        assert.equal(statuses.length, 0);

        return repo.getCommit(cherrypickOid);
      })
      .then(function(commit) {
        var opts = {
          mainline: 1
        };

        return Cherrypick.cherrypick(repo, commit, opts);
      })
      .then(function() {
        return repo.getStatus();
      })
      .then(function(statuses) {
        assert.equal(statuses.length, 1);

        return fse.readFile(path.join(workDirPath, repoInfo.ourFileName));
      })
      .then(function(fileContent) {
        assert.equal(fileContent, repoInfo.ourFileContent + addedContent);
      });
  });
});
