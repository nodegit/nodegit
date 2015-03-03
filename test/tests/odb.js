var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Odb", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Oid = NodeGit.Oid;
  var Obj = NodeGit.Object;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repo) {
      test.repo = repo;

      return repo;
    }).then(function(repo) {
      return repo.odb();
    }).then(function(odb) {
      test.odb = odb;

      return odb;
    });
  });

  it("can read raw objects directly from the odb using an OID", function() {
    var oid = Oid.fromString("32789a79e71fbc9e04d3eff7425e1771eb595150");

    return this.odb.read(oid)
      .then(function (object) {
        assert.equal(object.type(), Obj.TYPE.COMMIT);
      });
  });

  it("can read objects directly from the odb using a string", function() {
    return this.odb.read("32789a79e71fbc9e04d3eff7425e1771eb595150")
      .then(function (object) {
        assert.equal(object.type(), Obj.TYPE.COMMIT);
      });
  });

  it("can write raw objects to git", function() {
    var obj = "test data";
    var odb = this.odb;

    return odb.write(obj, obj.length, Obj.TYPE.BLOB)
      .then(function(oid) {
        assert.ok(oid instanceof Oid);

        return odb.read(oid);
      })
      .then(function(object) {
        assert.equal(object.type(), Obj.TYPE.BLOB);
        assert.equal(object.toString(), obj);
        assert.equal(object.size(), obj.length);
      });
  });
});
