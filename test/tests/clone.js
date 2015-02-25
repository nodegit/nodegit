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
    if (this.clonePath) {
      return fse.remove(this.clonePath).catch(function(err) {
        console.log(err);

        throw err;
      });
    }
  });

  afterEach(function(done) {
    if (this.repository) {
      this.repository.stateCleanup();
      this.repository.free();
      delete this.repository;
    }

    process.nextTick(function() {
      global.gc();
      done();
    });
  });

  it.skip("can clone with http", function() {
    var test = this;
    var url = "http://github.com/nodegit/test.git";
    var opts = {
      remoteCallbacks: {
        certificateCheck: function() {
          return 1;
        }
      }
    };

    test.clonePath = local("../repos/http");

    return Clone.clone(url, test.clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  it("can clone with https", function() {
    var test = this;
    var url = "https://github.com/nodegit/test.git";
    var opts = {
      remoteCallbacks: {
        certificateCheck: function() {
          return 1;
        }
      }
    };

    test.clonePath = local("../repos/https");

    return Clone.clone(url, test.clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  it("can clone with ssh", function() {
    var test = this;
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

    test.clonePath = local("../repos/ssh");

    return Clone.clone(url, test.clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  it("can clone with ssh while manually loading a key", function() {
    var test = this;
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

    test.clonePath = local("../repos/sshManual");

    return Clone.clone(url, test.clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  it("can clone with git", function() {
    var test = this;
    var url = "git://github.com/nodegit/test.git";
    var opts = {
      remoteCallbacks: {
        certificateCheck: function() {
          return 1;
        }
      }
    };

    test.clonePath = local("../repos/git");

    return Clone.clone(url, test.clonePath, opts).then(function(repo) {
      test.repository = repo;
      assert.ok(repo instanceof Repository);
    });
  });

  it("can clone with filesystem", function() {
    var test = this;
    var prefix = process.platform === "win32" ? "" : "file://";
    var url = prefix + local("../repos/empty");

    test.clonePath = local("../repos/filesystem");

    return Clone.clone(url, test.clonePath).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
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
