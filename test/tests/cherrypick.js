var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var Promise = require("bluebird");
var promisify = require("thenify-all");
var fse = promisify(require("fs-extra"),
  ["remove", "ensureDir", "writeFile", "readFile"]);

describe("Cherrypick", function() {
  var NodeGit = require("../../");
  var Cherrypick = NodeGit.Cherrypick;

  var repoPath = local("../repos/cherrypick");

  beforeEach(function() {
    var test = this;
    return fse.remove(repoPath)
      .then(function() {
        return fse.ensureDir(repoPath);
      })
      .then(function() {
        return NodeGit.Repository.init(repoPath, 0);
      })
      .then(function(repo) {
        test.repository = repo;
      });
  });

  after(function() {
    return fse.remove(repoPath);
  });

  function addFileToIndex(repository, fileName) {
    return repository.openIndex()
      .then(function(index) {
        index.read(1);
        index.addByPath(fileName);
        index.write();

        return index.writeTree();
      });
  }

  // Expects empty repo
  function setupBranches(repository, checkoutOurs) {
    var repoWorkDir = repository.workdir();

    var ourBranchName = "ours";
    var theirBranchName = "theirs";

    var baseFileName = "baseNewFile.txt";
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var baseFileContent = "How do you feel about Toll Roads?";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var initialCommit;
    var ourBranch;
    var theirBranch;

    var ret = {
      ourBranchName: ourBranchName,
      theirBranchName: theirBranchName,

      ourSignature: ourSignature,
      theirSignature: theirSignature,

      ourFileName: ourFileName,
      theirFileName: theirFileName,

      ourFileContent: ourFileContent,
      theirFileContent: theirFileContent
    };

    return Promise.all([
      fse.writeFile(path.join(repoWorkDir, baseFileName), baseFileContent),
      fse.writeFile(path.join(repoWorkDir, ourFileName), ourFileContent),
      fse.writeFile(path.join(repoWorkDir, theirFileName), theirFileContent)
    ])
      .then(function() {
        return addFileToIndex(repository, baseFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "b5cdc109d437c4541a13fb7509116b5f03d5039a");

        return repository.createCommit("HEAD", ourSignature, ourSignature,
          "initial commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "be03abdf0353d05924c53bebeb0e5bb129cda44a");

        return repository.getCommit(commitOid);
      })
      .then(function(commit) {
        ret.initialCommit = initialCommit = commit;

        return Promise.all([
          repository.createBranch(ourBranchName, initialCommit),
          repository.createBranch(theirBranchName, initialCommit)
        ]);
      })
      .then(function(branches) {
        assert(branches[0]);
        assert(branches[1]);

        ret.ourBranch = ourBranch = branches[0];
        ret.theirBranch = theirBranch = branches[1];

        return addFileToIndex(repository, ourFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "77867fc0bfeb3f80ab18a78c8d53aa3a06207047");

        return repository.createCommit(ourBranch.name(), ourSignature,
          ourSignature, "we made a commit", oid, [initialCommit]);
      })
      .then(function(commitOid) {
        return repository.getCommit(commitOid);
      })
      .then(function(commit) {
        ret.ourCommit = commit;
        return NodeGit.Reset.default(repository, initialCommit, ourFileName);
      })
      .then(function() {
        return addFileToIndex(repository, theirFileName);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "be5f0fd38a39a67135ad68921c93cd5c17fefb3d");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [initialCommit]);
      })
      .then(function(commitOid) {
        return repository.getCommit(commitOid);
      })
      .then(function(commit) {
        ret.theirCommit = commit;
        return NodeGit.Reset.default(repository, initialCommit, theirFileName);
      })
      .then(function() {
        return Promise.all([
          fse.remove(path.join(repoWorkDir, ourFileName)),
          fse.remove(path.join(repoWorkDir, theirFileName))
        ]);
      })
      .then(function() {
        if (checkoutOurs) {
          var opts = {
            checkoutStrategy: NodeGit.Checkout.STRATEGY.FORCE
          };

          return repository.checkoutBranch(ourBranchName, opts);
        }
      })
      .then(function() {
        return ret;
      });
  }

  it("can cherrypick a commit onto the index", function() {
    var repo = this.repository;
    var workDirPath = repo.workdir();
    var repoInfo;

    return setupBranches(repo, true)
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
      });
  });

  it("can cherrypick a commit onto another specified commit", function() {
    var repo = this.repository;
    var workDirPath = repo.workdir();
    var repoInfo;

    return setupBranches(repo)
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
        index.write();

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

    return setupBranches(repo, true)
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
