var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

var exec = require("../../utils/execPromise");

describe("Config", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Config = NodeGit.Config;

  var reposPath = local("../repos/workdir");

  describe("openOnDisk", function() {
    var configPath = path.join(reposPath, ".git/config");

    it("opens the same config as the repo", function() {
      var repo;
      var onDiskConfig;
      return Repository.open(reposPath)
        .then(function(_repo) {
          repo = _repo;
          return Config.openOndisk(configPath);
        })
        .then(function(config) {
          onDiskConfig = config;
          return repo.config();
        })
        .then(function(repoConfig) {
          return Promise.all([
            onDiskConfig.getString("core.filemode"),
            onDiskConfig.getString("core.bare"),
            repoConfig.getString("core.filemode"),
            repoConfig.getString("core.bare")
          ]);
        })
        .then(function(results) {
          var onDiskFileMode = results[0];
          var onDiskBare = results[1];
          var repoFileMode = results[2];
          var repoBare = results[3];

          assert.equal(onDiskFileMode, repoFileMode);
          assert.equal(onDiskBare, repoBare);
        });
    });

    it("opens the config and can change a value", function() {
      var repo;
      var onDiskConfig;
      var repoConfig;
      var originalFileMode;
      return Repository.open(reposPath)
        .then(function(_repo) {
          repo = _repo;
          return Config.openOndisk(configPath);
        })
        .then(function(config) {
          onDiskConfig = config;
          return repo.config();
        })
        .then(function(_repoConfig) {
          repoConfig = _repoConfig;
          return Promise.all([
            onDiskConfig.getString("core.filemode"),
            repoConfig.getString("core.filemode")
          ]);
        })
        .then(function(results) {
          var onDiskFileMode = results[0];
          var repoFileMode = results[1];

          assert.equal(onDiskFileMode, repoFileMode);
          originalFileMode = onDiskFileMode;
          var oppositeFileMode = onDiskFileMode === "true" ? "false" : "true";

          return onDiskConfig.setString(
            "core.filemode",
            oppositeFileMode
          );
        })
        .then(function() {
          return Config.openOndisk(configPath);
        })
        .then(function(config) {
          return Promise.all([
            config.getString("core.filemode"),
            repoConfig.getString("core.filemode")
          ]);
        })
        .then(function(results) {
          var onDiskFileMode = results[0];
          var repoFileMode = results[1];
          assert.notEqual(onDiskFileMode, originalFileMode);
          assert.equal(onDiskFileMode, repoFileMode);
        })
        .then(function() {
          // cleanup
          return onDiskConfig.setString(
            "core.filemode",
            originalFileMode
          );
        });
    });
  });

  it("can get and set a global value", function() {
    var savedUserName;

    function finallyFn() {
      return exec("git config --global user.name \"" + savedUserName + "\"");
    }

    return exec("git config --global user.name")
      .then(function(userName) {
        savedUserName = userName.trim();

        return exec(
          "git config --global user.name \"" + savedUserName + "-test\"");
      })
      .then(function() {
        return NodeGit.Config.openDefault();
      })
      .then(function(config) {
        return config.getString("user.name");
      })
      .then(function(userNameFromNodeGit) {
        assert.equal(savedUserName + "-test", userNameFromNodeGit);
      })
      .then(finallyFn)
      .catch(function(e) {
        return finallyFn()
          .then(function() {
            throw e;
          });
      });
  });

  it("will reject when getting value of non-existent config key", function() {
    // Test initially for finding source of a segfault. There was a problem
    // where getting an empty config value crashes nodegit.
    return NodeGit.Config.openDefault()
      .then(function(config) {
        return config.getString("user.fakevalue");
      })
      .catch(function (e) {
        return true;
      });
  });

  it("can get and set a repo config value", function() {
    var savedUserName;

    function finallyFn() {
      return exec("git config user.name \"" + savedUserName + "\"", {
        cwd: reposPath
      });
    }

    return exec("git config user.name", {
      cwd: reposPath
    })
    .then(function(userName) {
      savedUserName = userName.trim();

      return exec("git config user.name \"" + savedUserName + "-test\"", {
        cwd: reposPath
      });
    })
    .then(function() {
      return NodeGit.Repository.open(reposPath);
    })
    .then(function(repo) {
      return repo.config();
    })
    .then(function(config) {
      return config.getString("user.name");
    })
    .then(function(userNameFromNodeGit) {
      assert.equal(savedUserName + "-test", userNameFromNodeGit);
    })
    .then(finallyFn)
    .catch(function(e) {
      return finallyFn()
        .then(function() {
          throw e;
        });
    });
  });

  describe("getPath", function() {
    it("can get path for a given config", function() {
      return NodeGit.Repository.open(reposPath)
        .then(function(repo) {
          return repo.config();
        })
        .then(function(config) {
          return Promise.all([
            config.getPath("core.filemode"),
            config.getString("core.filemode")
          ]);
        })
        .then(function(results) {
          var localFilemode = results[0];
          var repoFilemode = results[1];
          assert.equal(localFilemode, repoFilemode);
        });
    });
  });
});
