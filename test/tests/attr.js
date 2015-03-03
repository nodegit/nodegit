var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Attr", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Attr = NodeGit.Attr;
  var Status = NodeGit.Status;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      });
  });

  it("can add a macro definition", function() {
    var error = Attr.addMacro(this.repository, "binary", "-diff -crlf");

    assert.equal(error, 0);
  });

  it("can flush the attr cache", function() {
    Attr.cacheFlush(this.repository);
  });

  it("can lookup the value of a git attribute", function() {
    var flags = Status.SHOW.INDEX_AND_WORKDIR;
    return Attr.get(this.repository, flags, ".gitattributes", "test");
  });
});
