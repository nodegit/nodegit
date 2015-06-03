var assert = require("assert");

describe("StatusFile", function() {
  var NodeGit = require("../../");
  var Status = NodeGit.Status;
  var StatusFile = NodeGit.StatusFile;

  var pathName = "README.md";

  function testStatusFile(status) {
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
  }

  it.skip("identifies the proper statuses for CURRENT", function() {
    testStatusFile("CURRENT");
  });

  it.skip("identifies the proper statuses for WT_UNREADABLE", function() {
    testStatusFile("WT_UNREADABLE");
  });

  it("identifies the proper statuses for WT_NEW", function() {
    testStatusFile("WT_NEW");
  });

  it("identifies the proper statuses for WT_MODIFIED", function() {
    testStatusFile("WT_MODIFIED");
  });

  it("identifies the proper statuses for WT_DELETED", function() {
    testStatusFile("WT_DELETED");
  });

  it("identifies the proper statuses for WT_TYPECHANGE", function() {
    testStatusFile("WT_TYPECHANGE");
  });

  it("identifies the proper statuses for WT_RENAMED", function() {
    testStatusFile("WT_RENAMED");
  });

  it("identifies the proper statuses for IGNORED", function() {
    testStatusFile("IGNORED");
  });

  it("identifies the proper statuses for INDEX_NEW", function() {
    testStatusFile("INDEX_NEW");
  });

  it("identifies the proper statuses for INDEX_MODIFIED", function() {
    testStatusFile("INDEX_MODIFIED");
  });

  it("identifies the proper statuses for INDEX_DELETED", function() {
    testStatusFile("INDEX_DELETED");
  });

  it("identifies the proper statuses for INDEX_TYPECHANGE", function() {
    testStatusFile("INDEX_TYPECHANGE");
  });

  it("identifies the proper statuses for INDEX_RENAMED", function() {
    testStatusFile("INDEX_RENAMED");
  });

});
