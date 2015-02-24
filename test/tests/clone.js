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
  var git = local("../repos/git");
  var file = local("../repos/file");

  var sshPublicKey = local("../id_rsa.pub");
  var sshPrivateKey = local("../id_rsa");

  // Set a reasonable timeout here now that our repository has grown.
  this.timeout(15000);

  beforeEach(function() {
    return NodeGit.Promise.all([
      fse.remove(http),
      fse.remove(https),
      fse.remove(ssh),
      fse.remove(git),
      fse.remove(file)
    ]);
  });

  it.skip("can clone with http", function() {
    var url = "http://github.com/nodegit/test.git";
    var opts = {
      remoteCallbacks: {
        certificateCheck: function() {
          return 1;
        }
      }
    };

    return Clone.clone(url, http, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
    });
  });

  it("can clone with https", function() {
    var url = "https://github.com/nodegit/test.git";
    var opts = {
      remoteCallbacks: {
        certificateCheck: function() {
          return 1;
        }
      }
    };

    return Clone.clone(url, https, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
    });
  });

  it("can clone with ssh", function() {
    var url = "git@github.com:nodegit/test.git";
    var opts = {
      remoteCallbacks: {
        certificateCheck: function() {
          return 1;
        },
        credentials: function(url, userName) {
          return NodeGit.Cred.sshKeyFromAgent(userName);
        }
      }
    };

    return Clone.clone(url, ssh, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
    });
  });

  it("can clone with ssh while manually loading a key", function() {
    var url = "git@github.com:nodegit/test.git";
    var opts = {
      remoteCallbacks: {
        certificateCheck: function() {
          return 1;
        },
        credentials: function(url, userName) {
          return NodeGit.Cred.sshKeyNew(
            userName,
            sshPublicKey,
            sshPrivateKey,
            "");
        }
      }
    };

    return Clone.clone(url, ssh, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
    });
  });

  it("can clone with git", function() {
    var url = "git://github.com/nodegit/test.git";
    var opts = { 
      remoteCallbacks: {
        certificateCheck: function() {
          return 1;
        }
      }
    };

    return Clone.clone(url, git, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
    });
  });

  it("can clone with filesystem", function() {
    var prefix = process.platform === "win32" ? "" : "file://";
    var url = prefix + local("../repos/empty");

    return Clone.clone(url, file).then(function(repo) {
      assert.ok(repo instanceof Repository);
    });
  });

  it("will not segfault when accessing a url without username", function() {
    var url = "https://github.com/nodegit/private";

    return Clone.clone(url, git, {
      remoteCallbacks: {
        certificateCheck: function() {
          return 1;
        },
        credentials: function() {
          return NodeGit.Cred.userpassPlaintextNew("fake-token",
            "x-oauth-basic");
        }
      }
    }).catch(function unhandledError() { });
  });
});
