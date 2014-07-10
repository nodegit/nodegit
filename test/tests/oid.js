var assert = require("assert");

var NodeGit = require("../../");

describe("Oid", function() {
  var oid = "fce88902e66c72b5b93e75bdb5ae717038b221f6";

  var Oid = require("../../lib/oid");

  before(function() {
    this.oid = Oid.fromstr(oid);
  });

  it("can convert a string to an oid", function() {
    assert.ok(this.oid instanceof NodeGit.Oid);
  });

  it("can convert an oid to a string", function() {
    var string = this.oid.allocfmt();

    assert.equal(string, oid);
  });

  it("provides a custom inspect method to improve debugging", function() {
    var inspect = this.oid.inspect();

    assert.equal(inspect, "[Oid " + oid + "]");
  });
});
