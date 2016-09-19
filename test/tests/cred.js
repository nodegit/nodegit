var assert = require("assert");
var path = require("path");
var fs = require("fs");
var local = path.join.bind(path, __dirname);

describe("Cred", function() {
  var NodeGit = require("../../");
  
  var sshPublicKey = local("../id_rsa.pub");
  var sshPrivateKey = local("../id_rsa");

  it("can create default credentials", function() {
    var defaultCreds = NodeGit.Cred.defaultNew();
    assert.ok(defaultCreds instanceof NodeGit.Cred);
  });

  it("can create ssh credentials using passed keys", function() {
    var cred = NodeGit.Cred.sshKeyNew(
      "username",
      sshPublicKey,
      sshPrivateKey,
      "");

    assert.ok(cred instanceof NodeGit.Cred);
  });

  it("can create ssh credentials using passed keys in memory", function() {
    var publicKeyContents = fs.readFileSync(sshPublicKey, {
      encoding: "ascii"
    });
    var privateKeyContents = fs.readFileSync(sshPrivateKey, {
      encoding: "ascii"
    });

    return NodeGit.Cred.sshKeyMemoryNew(
      "username",
      publicKeyContents,
      privateKeyContents,
      "").then(function(cred) {
        assert.ok(cred instanceof NodeGit.Cred);
      });
  });

  it("can create credentials using plaintext", function() {
    var plaintextCreds = NodeGit.Cred.userpassPlaintextNew
      ("username", "password");
    assert.ok(plaintextCreds instanceof NodeGit.Cred);
  });
  
  it("can create credentials using agent", function() {
    var fromAgentCreds = NodeGit.Cred.sshKeyFromAgent
      ("username");
    assert.ok(fromAgentCreds instanceof NodeGit.Cred);
  });

  it("can create credentials using username", function() {
    return NodeGit.Cred.usernameNew
      ("username").then(function(cred) {
        assert.ok(cred instanceof NodeGit.Cred);
      });
  });

  it("can return 1 if a username exists", function() {
    var plaintextCreds = NodeGit.Cred.userpassPlaintextNew
      ("username", "password");
    assert.ok(plaintextCreds.hasUsername() === 1);
  });

  it("can return 0 if a username does not exist", function() {
    var defaultCreds = NodeGit.Cred.defaultNew();
    assert.ok(defaultCreds.hasUsername() === 0);
  });
});
