var path = require("path");
var assert = require("assert");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));
var local = path.join.bind(path, __dirname);

describe("Clone", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Clone = NodeGit.Clone;

  var clonePath = local("../repos/clone");

  var sshPublicKey = local("../id_rsa.pub");
  var sshPrivateKey = local("../id_rsa");

  // Set a reasonable timeout here now that our repository has grown.
  this.timeout(30000);

  beforeEach(function() {
    return fse.remove(clonePath).catch(function(err) {
      console.log(err);

      throw err;
    });
  });

  it("can clone with http", function() {
    var test = this;
    var url = "http://git.tbranyen.com/smart/site-content";

    return Clone(url, clonePath).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  it("can clone with https", function() {
    var test = this;
    var url = "https://github.com/nodegit/test.git";
    var opts = {
        fetchOpts: {
          callbacks: {
            certificateCheck: function() {
              return 1;
          }
        }
      }
    };

    return Clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  it("can clone using nested function", function() {
    var test = this;
    var url = "https://github.com/nodegit/test.git";
    var opts = {
      fetchOpts: {
        callbacks: {
          certificateCheck: function() {
            return 1;
          }
        }
      }
    };

    return Clone.clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  it("can clone with ssh", function() {
    var test = this;
    var url = "git@github.com:nodegit/test.git";
    var opts = {
      fetchOpts: {
        callbacks: {
          certificateCheck: function() {
            return 1;
          },
          credentials: function(url, userName) {
            return NodeGit.Cred.sshKeyFromAgent(userName);
          }
        }
      }
    };

    return Clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  it("can clone with ssh while manually loading a key", function() {
    var test = this;
    var url = "git@github.com:nodegit/test.git";
    var opts = {
      fetchOpts: {
        callbacks: {
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
      }
    };

    return Clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  it("can clone with git", function() {
    var test = this;
    var url = "git://github.com/nodegit/test.git";
    var opts = {
      fetchOpts: {
        callbacks: {
          certificateCheck: function() {
            return 1;
          }
        }
      }
    };

    return Clone(url, clonePath, opts).then(function(repo) {
      test.repository = repo;
      assert.ok(repo instanceof Repository);
    });
  });

  it("can clone with filesystem", function() {
    var test = this;
    var prefix = process.platform === "win32" ? "" : "file://";
    var url = prefix + local("../repos/empty");

    return Clone(url, clonePath).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  it("will not segfault when accessing a url without username", function() {
    var url = "https://github.com/nodegit/private";

    var firstPass = true;

    return Clone(url, clonePath, {
      fetchOpts: {
        callbacks: {
          certificateCheck: function() {
            return 1;
          },
          credentials: function() {
            if (firstPass) {
              firstPass = false;
              return NodeGit.Cred.userpassPlaintextNew("fake-token",
                "x-oauth-basic");
            } else {
              return NodeGit.Cred.defaultNew();
            }
          }
        }
      }
    }).catch(function(reason) { });
  });
});
