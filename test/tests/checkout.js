var assert = require("assert");
var path = require("path");

describe("Checkout", function() {
  var packageJsonOid = "0fa56e90e096a4c24c785206b826ab914ea3de1e";
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Repository = require("../../lib/repository");
  var Checkout = require("../../lib/checkout");

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repo) {
      test.repo = repo;
    });
  });

  it("can checkout the head", function() {
    var repo = this.repo;

    Checkout.head(repo)
    .then(function() {
      return repo.getBlob(packageJsonOid);
    })
    .then(function(blob) {
      var packageJson = blob.toString();

      assert.ok(~packageJson.indexOf("\"ejs\": \"~1.0.0\","));
    });
  });
});
