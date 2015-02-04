var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Index", function() {
  var reposPath = local("../repos/workdir/.git");

  var Repository = require(local("../../lib/repository"));

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repo) {
      test.repo = repo;

      return repo.openIndex().then(function(index) {
        test.index = index;

        return index;
      });
    });
  });

  it("can get the index of a repo and examine entries", function() {
    var entries = this.index.entries();

    assert.equal(entries[0].path(), ".gitignore");
  });
});
