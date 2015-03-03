var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("StatusFile", function() {
  var NodeGit = require(local("../../"));
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
});
