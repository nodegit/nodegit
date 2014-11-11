var assert = require("assert");

describe("Cred", function() {
  var NodeGit = require("../../");

  it("can create default credentials", function() {
    NodeGit.Cred.defaultNew().then(function (defaultCreds) {
      assert(defaultCreds instanceof NodeGit.Cred);
    });
  });

  it("can create ssh credentials using passed keys", function() {
    NodeGit.Cred.sshKeyNew(
      "username",
      "public key",
      "private key",
      "passphrase")
    .then(function (sshCreds) {
      assert(sshCreds instanceof NodeGit.Cred);
    });
  });

  it("can create credentials using plaintext", function() {
    NodeGit.Cred.userpassPlaintextNew("username", "password")
    .then(function (plaintextCreds) {
      assert(plaintextCreds instanceof NodeGit.Cred);
    });
  });
});
