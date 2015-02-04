var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Cred", function() {
  var NodeGit = require(local("../../"));
  var sshPublicKey = local("../id_rsa.pub");
  var sshPrivateKey = local("../id_rsa");

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
