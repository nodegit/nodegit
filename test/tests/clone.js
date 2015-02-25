var path = require("path");
var assert = require("assert");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));
var local = path.join.bind(path, __dirname);

describe("Clone", function() {
  var Repository = require(local("../../lib/repository"));
  var Clone = require(local("../../lib/clone"));
  var NodeGit = require(local("../../"));

  var clonePath = local("../repos/clone");

  var sshPublicKey = local("../id_rsa.pub");
  var sshPrivateKey = local("../id_rsa");

  // Set a reasonable timeout here now that our repository has grown.
  this.timeout(30000);

  beforeEach(function() {
    return NodeGit.Promise.all([
      fse.remove(clonePath),
    ]).catch(function unhandledFunction(ex) {
      console.log(ex.message);
    });
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

    return Clone.clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      repo.free();
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

    return Clone.clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      repo.stateCleanup();
      repo.free();
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

    return Clone.clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      repo.stateCleanup();
      repo.free();
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

    return Clone.clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      repo.stateCleanup();
      repo.free();
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

    return Clone.clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      repo.stateCleanup();
      repo.free();
    });
  });

  it("can clone with filesystem", function() {
    var prefix = process.platform === "win32" ? "" : "file://";
    var url = prefix + local("../repos/empty");

    return Clone.clone(url, clonePath).then(function(repo) {
      assert.ok(repo instanceof Repository);
      repo.stateCleanup();
      repo.free();
    });
  });

  it("will not segfault when accessing a url without username", function() {
    var url = "https://github.com/nodegit/private";

    return Clone.clone(url, clonePath, {
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
