var assert = require("assert");
var path = require("path");

describe("Libgit2", function() {
  var NodeGit = require("../../");
  var Libgit2 = NodeGit.Libgit2;
  var certPath = path.resolve("../certs");
  var certFile = path.join(certPath, "fakecert.crt");

  if (process.platform !== "win32") {
    describe("setSslLocations", function() {
      it("can set the CA info in libgit2", function() {
        return Libgit2.setSslLocations(certFile, null)
          .then(function() {
            // It worked, we need to pass the test
            assert.equal(0, 0);
          });
      });

      it("can set the CA path in libgit2", function() {
        return Libgit2.setSslLocations(null, certPath)
          .then(function() {
            // It worked, we need to pass the test
            assert.equal(0, 0);
          });
      });
    });
  }
});
