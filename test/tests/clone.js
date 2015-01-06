var assert = require("assert");
var path = require("path");
var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));

describe("Clone", function() {
  var http = path.resolve("test/repos/http");
  var https = path.resolve("test/repos/https");
  var ssh = path.resolve("test/repos/ssh");
  var sshManual = path.resolve("test/repos/sshmanual");
  var git = path.resolve("test/repos/git");
  var file = path.resolve("test/repos/file");

  var sshPublicKey = path.resolve("./id_rsa.pub");
  var sshPrivateKey = path.resolve("./id_rsa");

  var Repository = require("../../lib/repository");
  var Clone = require("../../lib/clone");
  var NodeGit = require("../../");

  // Set a reasonable timeout here now that our repository has grown.
  this.timeout(15000);

  before(function() {
    return Promise.all([
      fse.remove(http),
      fse.remove(https),
      fse.remove(ssh),
      fse.remove(git),
      fse.remove(file),
    ]);
  });

  it("can clone with http", function() {
    var url = "http://github.com/nodegit/test.git";
    var opts = { ignoreCertErrors: 1 };

    return Clone.clone(url, http, opts).then(function(repository) {
      assert.ok(repository instanceof Repository);
    });
  });

  it("can clone with https", function() {
    var url = "https://github.com/nodegit/test.git";
    var opts = { ignoreCertErrors: 1 };

    return Clone.clone(url, https, opts).then(function(repository) {
      assert.ok(repository instanceof Repository);
    });
  });

  it("can clone with ssh", function() {
    var url = "git@github.com:nodegit/test.git";
    var opts = {
      ignoreCertErrors: 1,
      remoteCallbacks: {
        credentials: function(url, userName) {
          return NodeGit.Cred.sshKeyFromAgent(userName);
        }
      }
    };

    return Clone.clone(url, ssh, opts).then(function(repository) {
      assert.ok(repository instanceof Repository);
    });
  });

  it("can clone with ssh while manually loading a key", function() {
    var url = "git@github.com:nodegit/test.git";
    var opts = {
      ignoreCertErrors: 1,
      remoteCallbacks: {
        credentials: function(url, userName) {
          return NodeGit.Cred.sshKeyNew(
            userName,
            sshPublicKey,
            sshPrivateKey,
            "");
        }
      }
    };

    return Clone.clone(url, sshManual, opts).then(function(repository) {
      assert.ok(repository instanceof Repository);
    });
  });

  it("can clone with git", function() {
    var url = "git://github.com/nodegit/test.git";
    var opts = { ignoreCertErrors: 1 };

    return Clone.clone(url, git, opts).then(function(repository) {
      assert.ok(repository instanceof Repository);
    });
  });

  it("can clone with filesystem", function() {
    var prefix = process.platform === "win32" ? "" : "file://";
    var url = prefix + path.resolve("test/repos/empty");

    return Clone.clone(url, file).then(function(repository) {
      assert.ok(repository instanceof Repository);
    });
  });
});
