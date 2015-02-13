var path = require("path");
var assert = require("assert");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));
var local = path.join.bind(path, __dirname);

describe("Clone", function() {
  var Repository = require(local("../../lib/repository"));
  var Clone = require(local("../../lib/clone"));
  var NodeGit = require(local("../../"));

  var http = local("../repos/http");
  var https = local("../repos/https");
  var ssh = local("../repos/ssh");
  var sshManual = local("../repos/sshmanual");
  var git = local("../repos/git");
  var file = local("../repos/file");

  var sshPublicKey = local("../id_rsa.pub");
  var sshPrivateKey = local("../id_rsa");

  // Set a reasonable timeout here now that our repository has grown.
  this.timeout(15000);

  function prepTestAndClean(url, location, opts) {
    return fse.remove(location)
      .then(function() {
        return Clone.clone(url, location, opts);
      })
      .then(function(repo) {
        assert.ok(repo instanceof Repository);
      });
  }

  it.skip("can clone with http", function() {
    var url = "http://github.com/nodegit/test.git";
    var opts = {
      remoteCallbacks: {
        certificateCheck: function() {
          return 1;
        }
      }
    };

    return prepTestAndClean(url, http, opts);
  });

  it.only("can clone with https", function() {
    var url = "https://github.com/nodegit/test.git";
    var opts = {
      remoteCallbacks: {
        certificateCheck: function() {
          return 1;
        }
      }
    };

    return prepTestAndClean(url, https, opts);
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

    return prepTestAndClean(url, ssh, opts);
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

    return prepTestAndClean(url, sshManual, opts);
  });

  it("can clone with git", function() {
    var url = "git://github.com/nodegit/test.git";
    var opts = { ignoreCertErrors: 1 };

    return prepTestAndClean(url, git, opts);
  });

  it("can clone with filesystem", function() {
    var prefix = process.platform === "win32" ? "" : "file://";
    var url = prefix + local("../repos/empty");

    return prepTestAndClean(url, file);
  });
});
