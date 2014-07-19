var path = require("path");

describe("Checkout", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Repository = require("../../lib/repository");

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;
    });
  });
});
