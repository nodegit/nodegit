var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("StatusFile", function() {
  var Status = require(local("../../lib/status"));
  var StatusFile = require(local("../../lib/status_file"));

  var pathName = "README.md";
  var statusCode = Status.STATUS.WT_NEW;

  var status = new StatusFile(pathName, statusCode);

  it("passes the path to the working function", function() {
    assert.equal(status.path(), pathName);
  });

  it("identifies the proper statuses", function() {
    assert.ok(status.isNew());
    assert.ok(!status.isModified());
  });
});
