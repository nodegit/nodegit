var assert = require("assert");
var path = require("path");

describe("Cred", function() {
  var NodeGit = require("../../");
  var sshPublicKey = path.resolve("./id_rsa.pub");
  var sshPrivateKey = path.resolve("./id_rsa");

  it("can create default credentials", function() {
    var defaultCreds = NodeGit.Cred.defaultNew();
    assert(defaultCreds instanceof NodeGit.Cred);
  });

  it("can create ssh credentials using passed keys", function() {
    var cred = NodeGit.Cred.sshKeyNew(
      "username",
      sshPublicKey,
      sshPrivateKey,
      "");

    assert(cred instanceof NodeGit.Cred);
  });

  it("can create credentials using plaintext", function() {
    var plaintextCreds = NodeGit.Cred.userpassPlaintextNew
      ("username", "password");
    assert(plaintextCreds instanceof NodeGit.Cred);
  });
});
