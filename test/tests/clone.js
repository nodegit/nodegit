var path = require("path");
var assert = require("assert");
var fse = require("fs-extra");
var local = path.join.bind(path, __dirname);
var _ = require("lodash");
const util = require("util");
const exec = util.promisify(require("child_process").exec);


const generatePathWithLength = (base, length) => {
  let path = `${base}/`;
  const baseLength = path.length;
  const remaining = length - baseLength;

  for (let i = 0; i < remaining; ++i) {
		// add a slash every 240 characters, but not as first or last character
    if (i % 239 == 0 && i != remaining - 1 && i != 0) {
      path += "/";
    } else {
      path += "a";
    }
  }

  assert.ok(path.length === length);

  return path;
};

describe("Clone", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Clone = NodeGit.Clone;

  var clonePath = local("../repos/clone");
  var longClonePath = generatePathWithLength(clonePath, 600);

  var sshPublicKeyPath = local("../id_rsa.pub");
  var sshPrivateKeyPath = local("../id_rsa");
  var sshEncryptedPublicKeyPath = local("../encrypted_rsa.pub");
  var sshEncryptedPrivateKeyPath = local("../encrypted_rsa");

  // Set a reasonable timeout here now that our repository has grown.
  this.timeout(30000);

  beforeEach(function() {
    return fse.remove(clonePath)
      .then(function() {
        return fse.remove(longClonePath);
      })
      .catch(function(err) {
        console.log(err);
        throw err;
      });
  });

  it.skip("can clone with http", function() {
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
            certificateCheck: () => 0
        }
      }
    };

    return Clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  it("can clone twice with https using same config object", function() {
    var test = this;
    var url = "https://github.com/nodegit/test.git";
    var progressCount = 0;
    var opts = {
      fetchOpts: {
        callbacks: {
          transferProgress: function(progress) {
            progressCount++;
          }
        }
      }
    };

    return Clone(url, clonePath, opts)
      .then(function(repo) {
        assert.ok(repo instanceof Repository);
        assert.notEqual(progressCount, 0);
        return fse.remove(clonePath);
      })
      .then(function() {
        progressCount = 0;
        return Clone(url, clonePath, opts);
      })
      .then(function(repo) {
        assert.ok(repo instanceof Repository);
        assert.notEqual(progressCount, 0);
        test.repository = repo;
      });
  });

  function updateProgressIntervals(progressIntervals, lastInvocation) {
    var now = new Date();
    if (lastInvocation) {
      progressIntervals.push(now - lastInvocation);
    }
    return now;
  }

  it("can clone with https and default throttled progress", function() {
    var test = this;
    var url = "https://github.com/nodegit/test.git";
    var progressCount = 0;
    var lastInvocation;
    var progressIntervals = [];
    var opts = {
        fetchOpts: {
          callbacks: {
            transferProgress: function(progress) {
              lastInvocation = updateProgressIntervals(progressIntervals,
                lastInvocation);
              progressCount++;
            }
          }
        }
    };

    return Clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      assert.notEqual(progressCount, 0);
      var averageProgressInterval = _.sum(progressIntervals) /
        progressIntervals.length;
      // even though we are specifying a throttle period of 100,
      // the throttle is applied on the scheduling side,
      // and actual execution is at the mercy of the main js thread
      // so the actual throttle intervals could be less than the specified
      // throttle period
      if (!averageProgressInterval || averageProgressInterval < 75) {
        assert.fail(averageProgressInterval, 75,
          "unexpected average time between callbacks", "<");
      }
      test.repository = repo;
    });
  });

  it("can clone with https and explicitly throttled progress", function() {
    var test = this;
    var url = "https://github.com/nodegit/test.git";
    var progressCount = 0;
    var lastInvocation;
    var progressIntervals = [];
    var opts = {
        fetchOpts: {
          callbacks: {
            transferProgress: {
              throttle: 50,
              callback: function(progress) {
                lastInvocation = updateProgressIntervals(progressIntervals,
                  lastInvocation);
                progressCount++;
              }
            }
          }
        }
    };

    return Clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      assert.notEqual(progressCount, 0);
      var averageProgressInterval = _.sum(progressIntervals) /
        progressIntervals.length;
      if (!averageProgressInterval || averageProgressInterval < 35) {
        assert.fail(averageProgressInterval, 35,
          "unexpected average time between callbacks", "<");
      }
      test.repository = repo;
    });
  });

  it("can clone without waiting for callback results", function() {
    var test = this;
    var url = "https://github.com/nodegit/test.git";
    var lastReceivedObjects = 0;
    var cloneFinished = false;
    var opts = {
        fetchOpts: {
          callbacks: {
            transferProgress: {
              waitForResult: false,
              callback: function(progress) {
                var receivedObjects = progress.receivedObjects();
                assert.false(
                  cloneFinished,
                  "callback running after clone completion"
                );
                assert.gt(receivedObjects, lastReceivedObjects);
                lastReceivedObjects = receivedObjects;
              }
            }
          }
        }
    };

    return Clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      cloneFinished = true;
      test.repository = repo;
    });
  });

  it("can clone using nested function", function() {
    var test = this;
    var url = "https://github.com/nodegit/test.git";
    var opts = {
      fetchOpts: {
        callbacks: {
          certificateCheck: () => 0
        }
      }
    };

    return Clone.clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  if (process.platform === "win32") {
    it("can clone with ssh using old agent with sha1 signing support only",
      async function () {
      var pageant = local("../../vendor/pageant.exe");
      var old_pageant = local("../../vendor/pageant_sha1.exe");
      var privateKey = local("../../vendor/private.ppk");
      var test = this;
      var url = "git@github.com:nodegit/test.git";
      var opts = {
        fetchOpts: {
          callbacks: {
            certificateCheck: () => 0,
            credentials: function(url, userName) {
              return NodeGit.Credential.sshKeyFromAgent(userName);
            }
          }
        }
      };

      try {
        await exec("taskkill /im pageant.exe /f /t");
      } catch (e) {
        try {
          await exec("taskkill /im pageant_sha1.exe /f /t");
        } catch(e) {}
      }
      try {
        await exec(`powershell -command "Start-Process ${old_pageant} ${privateKey}`);
      } catch (e) {
        try {
          await exec(`powershell -command "Start-Process ${pageant} ${privateKey}`);
        } catch (e) {}
        return assert.fail("Cannot run old pageant");
      }

      try {
        const repo = await Clone(url, clonePath, opts);
        test.repository = repo;
      } catch(e) {
        return assert.fail("Clone error: " + e.message);
      }

      try {
        await exec("taskkill /im pageant_sha1.exe /f /t");
      } catch(e) {}

      try {
        await exec(`powershell -command "Start-Process ${pageant} ${privateKey}`);
      } catch (e) {
        return assert.fail("Cannot run pageant");
      }

      return assert.ok(test.repository instanceof Repository);
    });
  }

  it("can clone with ssh", function() {
    var test = this;
    var url = "git@github.com:nodegit/test.git";
    var opts = {
      fetchOpts: {
        callbacks: {
          certificateCheck: () => 0,
          credentials: function(url, userName) {
            return NodeGit.Credential.sshKeyFromAgent(userName);
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
          certificateCheck: () => 0,
          credentials: function(url, userName) {
            return NodeGit.Credential.sshKeyNew(
              userName,
              sshPublicKeyPath,
              sshPrivateKeyPath,
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

  it("can clone with ssh while manually loading an encrypted key", function() {
    var test = this;
    var url = "git@github.com:nodegit/test.git";
    var opts = {
      fetchOpts: {
        callbacks: {
          certificateCheck: () => 0,
          credentials: function(url, userName) {
            return NodeGit.Credential.sshKeyNew(
              userName,
              sshEncryptedPublicKeyPath,
              sshEncryptedPrivateKeyPath,
              "test-password"
            );
          }
        }
      }
    };

    return Clone(url, clonePath, opts).then(function(repo) {
      assert.ok(repo instanceof Repository);
      test.repository = repo;
    });
  });

  // Since 15 March the unauthenticated git protocol on port 9418 is no longer supported in Github.
  // https://github.blog/2021-09-01-improving-git-protocol-security-github/
  it.skip("can clone with git", function() {
    var test = this;
    var url = "git://github.com/nodegit/test.git";
    var opts = {
      fetchOpts: {
        callbacks: {
          certificateCheck: () => 0
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
          certificateCheck: () => 0,
          credentials: function() {
            if (firstPass) {
              firstPass = false;
              return NodeGit.Credential.userpassPlaintextNew("fake-token",
                "x-oauth-basic");
            } else {
              return NodeGit.Credential.defaultNew();
            }
          }
        }
      }
    }).catch(function(reason) { });
  });
});
