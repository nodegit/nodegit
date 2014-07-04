var assert = require("assert");
var path = require("path");

var nodegit = require("../../");

describe("Blob", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Oid = nodegit.Oid;
  var Repository = nodegit.Repository;

  it("can fetch content from a commit", function(done) {
    var oid= Oid.fromstr("111dd657329797f6165f52f5085f61ac976dcf04");

    Repository.open(reposPath, function(err, repository) {
      repository.getBlob(oid, function(err, blob) {
        var contents = blob.toString();

        assert.equal(contents.slice(0, 7), "@import");

        done();
      });
    });
  });
});
