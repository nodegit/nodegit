var assert = require("assert");
var path = require("path");

var nodegit = require("../../");

describe("Blob", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Oid = nodegit.Oid;
  var Repository = nodegit.Repository;

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;
    });
  });

  it("can fetch content from a commit", function() {
    var oid= Oid.fromstr("111dd657329797f6165f52f5085f61ac976dcf04");

    return this.repository.getBlob(oid).then(function(blob) {
      assert.equal(blob.toString().slice(0, 7), "@import");
    });
  });
});
