var assert = require("assert");

describe("StatusFile", function() {
  var NodeGit = require("../../");
  var Status = NodeGit.Status;
  var StatusFile = NodeGit.StatusFile;

  var pathName = "README.md";
  var statusCode = Status.STATUS.WT_NEW;

  before(function() {
    this.status = new StatusFile({path: pathName, status: statusCode});
  });

  it("passes the path to the working function", function() {
    assert.equal(this.status.path(), pathName);
  });

  it("identifies the proper statuses", function() {
    assert.ok(this.status.isNew());
    assert.ok(!this.status.isModified());
  });

  it("detects working tree and index statuses", function() {
    assert.ok(this.status.inWorkingTree());
    assert.ok(!this.status.inIndex());
  });
});
