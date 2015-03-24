var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Packbuilder", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Packbuilder = NodeGit.Packbuilder;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;
    });
  });

  it("can be initialized", function() {
    var packBuilder = Packbuilder.create(this.repository);

    assert(packBuilder instanceof Packbuilder);
  });
});
