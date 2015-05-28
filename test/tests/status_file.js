var assert = require("assert");

describe("StatusFile", function() {
  var NodeGit = require("../../");
  var Status = NodeGit.Status;
  var StatusFile = NodeGit.StatusFile;

  var pathName = "README.md";

  var checkStatusFile = function(status) {
    it("identifies the proper statuses for " + status, function() {
      var statusFile = new StatusFile({
        path: pathName,
        status: Status.STATUS[status]
      });
      var specialFunction = status.replace(/^(WT|INDEX)_/, "");
      specialFunction = "is" +
        specialFunction[0] +
        specialFunction.substring(1).toLowerCase();
      if (/^WT_/.test(status)) {
        assert.ok(statusFile.inWorkingTree());
        assert.ok(!statusFile.inIndex());
      }
      if (/^INDEX_/.test(status)) {
        assert.ok(!statusFile.inWorkingTree());
        assert.ok(statusFile.inIndex());
      }
      assert.equal(statusFile.path(), pathName);
      assert.equal(statusFile.statusBit(), Status.STATUS[status]);
      assert.equal(statusFile.status(), status);
      assert.ok(statusFile[specialFunction]());
    });
  };

  for (var status in Status.STATUS) {
    if (status === "CURRENT" || status === "WT_UNREADABLE") {
      continue;
    }
    checkStatusFile(status);
  }


});
