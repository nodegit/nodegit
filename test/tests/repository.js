var assert = require("assert");
var path = require("path");

describe("Repository", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");
  var newRepo = path.resolve("test/repos/newrepo");
  var initRepo = path.resolve("test/repos/initrepo");

  var Repository = require("../../lib/repository");
  var Index = require("../../lib/index");

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;
    });
  });

  it("can open a valid repository", function() {
    assert.ok(this.repository instanceof Repository);
  });

  it("cannot open an invalid repository", function() {
    return Repository.open("repos/nonrepo").then(null, function(err) {
      assert.ok(err instanceof Error);
    });
  });

  it("does not try to open paths that don't exist", function() {
    var missingPath = "/surely/this/directory/does/not/exist/on/this/machine";

    return Repository.open(missingPath).then(null, function(err) {
      assert.ok(err instanceof Error);
    });
  });

  it("can initialize a repository into a folder", function() {
    return Repository.init(newRepo, 1).then(function(path, isBare) {
      return Repository.open(newRepo);
    });
  });

  it("can read the index", function() {
    return this.repository.index().then(function(index) {
      assert.ok(index instanceof Index);
    });
  });

  it("can list remotes", function() {
    return this.repository.getRemotes().then(function(remotes) {
      assert.equal(remotes.count(), 1);
      assert.equal(remotes.strings(), "origin");
    });
  });

  it("can utilize repository init options", function() {
    return Repository.initExt(initRepo, {});
  });
});
