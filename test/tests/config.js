var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var promisify = require("promisify-node");

// Have to wrap exec, since it has a weird callback signature.
var exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});

describe("Config", function() {
  var NodeGit = require("../../");

  var reposPath = local("../repos/workdir");

  it("can get and set a global value", function() {
    var savedUserName;
    var dummyUserName = "Dummy O'Fakey";

    function finallyFn() {
      return exec("git config --global user.name \"" + savedUserName + "\"");
    }

    return exec("git config --global user.name" + dummyUserName)
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
});
