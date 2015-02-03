var assert = require("assert");

describe("StatusFile", function() {
  var Status = require("../../lib/status");
  var StatusFile = require("../../lib/status_file");
  var path = "README.md";
  var statusCode = Status.STATUS.WT_NEW;

  var status = new StatusFile(path, statusCode);

  it("passes the path to the working function", function() {
    assert.equal(status.path(), path);
  });

  it("identifies the proper statuses", function() {
    assert.ok(status.isNew());
    assert.ok(!status.isModified());
  });
});
