var assert = require("assert");
var path = require("path");

describe("Revwalk", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");
  //var newRepo = path.resolve("test/repos/newrepo");

  var Repository = require("../../lib/repository");
  var Revwalk = require("../../lib/revwalk");
  var Oid = require("../../lib/oid");
  //var Index = require("../../lib/index");

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;
      test.walker = repository.createRevWalk();
    });
  });

  it("can create a walker", function() {
    assert.ok(this.walker instanceof Revwalk);
  });

  it("can push an object", function() {
    var test = this;

    return this.repository.getMaster().then(function(master) {
      test.walker.push(master.id());
      return test.walker.next().then(function(commit) {
        assert.equal(master.sha(), commit.toString());
      });
    });
  });

  it("can hide an object", function() {
    var test = this;

    var oid = Oid.fromString("1b27c70763f7b47c38d0c4e9b681415a08a3210e");
    test.walker.push(oid);
    test.walker.hide(Oid.fromstr("a03e044fcb45c654d4e15a4e495a6a0c6e632854"));
    return test.walker.next().then(function(commit) {
      return test.walker.next().then(function(commit) {
        assert.equal(commit.toString(),
          "32789a79e71fbc9e04d3eff7425e1771eb595150");
      });
    });
  });

  it("can simplify to first parent", function() {
    var test = this;

    var oid = Oid.fromString("1b27c70763f7b47c38d0c4e9b681415a08a3210e");
    test.walker.push(oid);
    test.walker.simplifyFirstParent();
    return test.walker.next().then(function(commit) {
      return test.walker.next().then(function(commit) {
        assert.equal(commit.toString(),
          "b8a94aefb22d0534cc0e5acf533989c13d8725dc");
      });
    });
  });
});
