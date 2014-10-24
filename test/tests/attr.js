var assert = require("assert");
var path = require("path");

describe("Attr", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Repository = require("../../lib/repository");
  var Attr = require("../../lib/attr");

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
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

  // FIXME Currently segfaults.
  it.skip("can lookup the value of a git attribute", function() {
    var flags = Attr.Check.NO_SYSTEM;
    var getAttr = Attr.get(this.repository, flags, ".gitattributes", "test");

    return getAttr.then(function(val) {
      console.log(val);
    }).catch(function(ex) {
      console.log(ex);
    });
  });
});
