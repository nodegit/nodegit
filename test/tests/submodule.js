var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Submodule", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var RepoUtils = require("../utils/repository_setup");
  var Submodule = NodeGit.Submodule;

  var repoPath = local("../repos/submodule");

  beforeEach(function() {
    var test = this;

    return RepoUtils.createRepository(repoPath)
      .then(function(repo) {
        test.repository = repo;
        return Repository.open(local("../repos/workdir"));
      })
      .then(function(repo) {
        test.workdirRepository = repo;
      });
  });

  it("can walk over the submodules", function() {
    var repo = this.workdirRepository;
    var submoduleName = "vendor/libgit2";

    return repo.getSubmoduleNames()
      .then(function(submodules) {
        assert.equal(submodules.length, 1);

        var submodule = submodules[0];
        assert.equal(submodule, submoduleName);
        return submodule;
      })
      .then(function(submodule) {
        return Submodule.lookup(repo, submodule);
      })
      .then(function(submodule) {
        assert.equal(submodule.name(), submoduleName);
      });
  });

  it("can get submodule status", function() {
    var repo = this.workdirRepository;
    var submoduleName = "vendor/libgit2";

    return Submodule.status(repo, submoduleName, Submodule.IGNORE.NONE)
      .then(function(status) {
        assert.equal(Submodule.STATUS.IN_CONFIG, status);
      });
  });

  it("can get submodule location", function() {
    var repo = this.workdirRepository;
    var submoduleName = "vendor/libgit2";

    return Submodule.lookup(repo, submoduleName)
      .then(function(submodule) {
        return submodule.location();
      })
      .then(function(status) {
        assert.equal(Submodule.STATUS.IN_CONFIG, status);
      });
  });

  it("can set submodule ignore", function() {
    var repo = this.workdirRepository;
    var submoduleName = "vendor/libgit2";

    return Submodule.setIgnore(repo, submoduleName, Submodule.IGNORE.ALL)
      .then(function() {
        return Submodule.lookup(repo, submoduleName);
      })
      .then(function(submodule) {
        assert.equal(Submodule.IGNORE.ALL, submodule.ignore());
      });
  });

  it("can set submodule url", function() {
    var repo = this.workdirRepository;
    var submoduleName = "vendor/libgit2";
    var submoduleUrl = "https://github.com/githubtraining/hellogitworld.git";

    return Submodule.setUrl(repo, submoduleName, submoduleUrl)
      .then(function() {
        return Submodule.lookup(repo, submoduleName);
      })
      .then(function(submodule) {
        assert.equal(submoduleUrl, submodule.url());
      });
  });

  it("can set submodule update", function() {
    var repo = this.workdirRepository;
    var submoduleName = "vendor/libgit2";

    return Submodule.setUpdate(repo, submoduleName, Submodule.UPDATE.NONE)
      .then(function() {
        return Submodule.lookup(repo, submoduleName);
      })
      .then(function(submodule) {
        assert.equal(Submodule.UPDATE.NONE, submodule.updateStrategy());
      });
  });

  it("can setup and finalize submodule add", function() {
    this.timeout(30000);

    var repo = this.repository;
    var submodulePath = "nodegittest";
    var submoduleUrl = "https://github.com/nodegit/test.git";

    var submodule;
    var submoduleRepo;

    return NodeGit.Submodule.addSetup(repo, submoduleUrl, submodulePath, 0)
      .then(function(_submodule) {
        submodule = _submodule;

        return submodule.init(0);
      })
      .then(function() {
        return submodule.open();
      })
      .then(function(_submoduleRepo) {
        submoduleRepo = _submoduleRepo;
        return submoduleRepo.fetch("origin", null, null);
      })
      .then(function() {
        return submoduleRepo.getReference("origin/master");
      })
      .then(function(reference) {
        return reference.peel(NodeGit.Object.TYPE.COMMIT);
      })
      .then(function(commit) {
        return submoduleRepo.createBranch("master", commit.id());
      })
      .then(function() {
        return submodule.addFinalize();
      })
      .then(function() {
        // check whether the submodule exists
        return Submodule.lookup(repo, submodulePath);
      })
      .then(function(submodule) {
        assert.equal(submodule.name(), submodulePath);
        // check whether .gitmodules and the submodule are in the index
        return repo.refreshIndex();
      })
      .then(function(index) {
        var entries = index.entries();
        assert.equal(entries.length, 2);
        assert.equal(entries[0].path, ".gitmodules");
        assert.equal(entries[1].path, submodulePath);
      });
  });

  it("can run sync callback without deadlocking", function() {
    var repo = this.workdirRepository;
    var submodules = [];
    var submoduleCallback = function(submodule, name, payload) {
      var submoduleName = submodule.name();
      assert.equal(submoduleName, name);
      submodules.push(name);
    };

    return Submodule.foreach(repo, submoduleCallback).then(function() {
      assert.equal(submodules.length, 1);
    });
  });

  // 'Submodule.foreach' and 'Submodule.lookup' do work with the repo locked.
  // They should work together without deadlocking.
  it("can run async callback without deadlocking", function() {
    var repo = this.workdirRepository;
    var submodules = [];
    var submoduleCallback = function(submodule, name, payload) {
      var owner = submodule.owner();

      return Submodule.lookup(owner, name)
        .then(function(submodule) {
          assert.equal(submodule.name(), name);
          submodules.push(name);
        });
    };

    return Submodule.foreach(repo, submoduleCallback).then(function() {
      assert.equal(submodules.length, 1);
    });
  });
});
