var assert = require("assert");

describe("Cred", function() {
  var NodeGit = require("../../");

  it("can create default credentials", function() {
    var defaultCreds = NodeGit.Cred.defaultNew();

    assert(defaultCreds instanceof NodeGit.Cred);
  });

  it("can create ssh credentials using passed keys", function() {
    var sshCreds
      = NodeGit.Cred.sshKeyNew(
        "username",
        "public key",
        "private key",
        "passphrase");

    assert(sshCreds instanceof NodeGit.Cred);
  });

  it("can create credentials using plaintext", function() {
    var plaintextCreds
      = NodeGit.Cred.userpassPlaintextNew("username", "password");

    assert(plaintextCreds instanceof NodeGit.Cred);
  });
});
