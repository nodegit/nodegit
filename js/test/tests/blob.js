var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Blob", function() {
  var NodeGit = require("../../");

  var Oid = NodeGit.Oid;
  var Repository = NodeGit.Repository;
  var FileMode = NodeGit.TreeEntry.FILEMODE;

  var reposPath = local("../repos/workdir");
  var oid = "111dd657329797f6165f52f5085f61ac976dcf04";

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;

        return repository.getBlob(oid);
      })
      .then(function(blob) {
        test.blob = blob;
      });
  });

  it("can provide content as a buffer", function() {
    var contents = this.blob.content();

    assert.ok(Buffer.isBuffer(contents));
  });

  it("can provide content as a string", function() {
    var contents = this.blob.toString();

    assert.equal(typeof contents, "string");
    assert.equal(contents.slice(0, 7), "@import");
  });

  it("can determine if a blob is not a binary", function() {
    assert.equal(this.blob.filemode(), FileMode.BLOB);
  });

  it("can get a blob with an Oid object", function() {
    var oidObject = Oid.fromString(oid);
    return this.repository.getBlob(oidObject)
      .then(function(blob) {
        assert.equal(blob.id().toString(), oid);
      });
  });
});
