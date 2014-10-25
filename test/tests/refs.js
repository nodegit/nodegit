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

  it("will fail looking up the target if not symbolic", function() {
    var refs = this.refs;

    assert.throws(function() {
      refs.symbolicTarget();
    });
  });

  it("can look up the HEAD sha", function() {
    return Refs.nameToId(this.repository, "HEAD").then(function(oid) {
      var sha = oid.allocfmt();

      assert.equal(sha, "79d86058c637000079d86058c637000079d86058");
    });
  });
});
