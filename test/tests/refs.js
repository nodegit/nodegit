var assert = require("assert");
var path = require("path");

describe("Refs", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Repository = require("../../lib/repository");
  var Refs = require("../../lib/refs");

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;

      return repository.getReference("refs/heads/master").then(function(refs) {
        test.refs = refs;
      });
    });
  });

  it("can look up a reference", function() {
    assert.ok(this.refs instanceof Refs);
  });

  it("can determine if the reference is symbolic", function() {
    assert.equal(this.refs.isSymbolic(), false);
  });

  it("will return undefined looking up the symbolic target if not symbolic",
    function() {
      var refs = this.refs;
      assert(refs.symbolicTarget() === undefined);
    });
});
