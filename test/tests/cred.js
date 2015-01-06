var assert = require("assert");
var path = require("path");

describe("Cred", function() {
  var NodeGit = require("../../");
  var sshPublicKey = path.resolve("./id_rsa.pub");
  var sshPrivateKey = path.resolve("./id_rsa");

  it("can create default credentials", function() {
    NodeGit.Cred.defaultNew().then(function (defaultCreds) {
      assert(defaultCreds instanceof NodeGit.Cred);
    });
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
    NodeGit.Cred.userpassPlaintextNew("username", "password")
    .then(function (plaintextCreds) {
      assert(plaintextCreds instanceof NodeGit.Cred);
    });
  });
});
