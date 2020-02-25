var assert = require("assert");
var path = require("path");
var fs = require("fs");
var local = path.join.bind(path, __dirname);

describe("Cred", function() {
  var NodeGit = require("../../");

  var sshPublicKey = local("../id_rsa.pub");
  var sshPrivateKey = local("../id_rsa");

  it("can create default credentials", function() {
    var defaultCreds = NodeGit.Credential.defaultNew();
    assert.ok(defaultCreds instanceof NodeGit.Credential);
  });

  it("can create ssh credentials using passed keys", function() {
    var cred = NodeGit.Credential.sshKeyNew(
      "username",
      sshPublicKey,
      sshPrivateKey,
      "");

    assert.ok(cred instanceof NodeGit.Credential);
  });

  it("can create ssh credentials using passed keys in memory", function() {
    var publicKeyContents = fs.readFileSync(sshPublicKey, {
      encoding: "ascii"
    });
    var privateKeyContents = fs.readFileSync(sshPrivateKey, {
      encoding: "ascii"
    });

    return NodeGit.Credential.sshKeyMemoryNew(
      "username",
      publicKeyContents,
      privateKeyContents,
      "").then(function(cred) {
        assert.ok(cred instanceof NodeGit.Credential);
      });
  });

  it("can create credentials using plaintext", function() {
    var plaintextCreds = NodeGit.Credential.userpassPlaintextNew
      ("username", "password");
    assert.ok(plaintextCreds instanceof NodeGit.Credential);
  });

  it("can create credentials using agent", function() {
    var fromAgentCreds = NodeGit.Credential.sshKeyFromAgent
      ("username");
    assert.ok(fromAgentCreds instanceof NodeGit.Credential);
  });

  it("can create credentials using username", function() {
    return NodeGit.Credential.usernameNew
      ("username").then(function(cred) {
        assert.ok(cred instanceof NodeGit.Credential);
      });
  });

  it("can return 1 if a username exists", function() {
    var plaintextCreds = NodeGit.Credential.userpassPlaintextNew
      ("username", "password");
    assert.ok(plaintextCreds.hasUsername() === 1);
  });

  it("can return 0 if a username does not exist", function() {
    var defaultCreds = NodeGit.Credential.defaultNew();
    assert.ok(defaultCreds.hasUsername() === 0);
  });
});
