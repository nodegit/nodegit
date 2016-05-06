var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

var leakTest = require("../utils/leak_test");

describe("Oid", function() {
  var NodeGit = require("../../");
  var Oid = NodeGit.Oid;

  var oid = "fce88902e66c72b5b93e75bdb5ae717038b221f6";

  before(function() {
    this.oid = Oid.fromString(oid);
  });

  it("can convert a string to an oid", function() {
    assert.ok(this.oid instanceof Oid);
  });

  it("can convert an oid to a string", function() {
    var string = this.oid.allocfmt();

    assert.equal(string, oid);
    assert.equal(this.oid.toString(), oid);
  });

  it("provides a custom inspect method to improve debugging", function() {
    var inspect = this.oid.inspect();

    assert.equal(inspect, "[Oid " + oid + "]");
  });

  it("can convert strings to oids in parameters", function() {
    return NodeGit.Repository.open(local("../repos/workdir"))
      .then(function(repo) {
        var revwalk = repo.createRevWalk();
        revwalk.sorting(NodeGit.Revwalk.SORT.TIME);

        revwalk.push(oid);

        return revwalk.getCommits(1);
      })
      .then(function(commits) {
        assert.equal(commits[0].toString(), oid);
      });
  });

  it("can compare two identical oids", function() {
    assert.equal(this.oid.cmp(this.oid), 0);
  });

  it("can compare two different oids", function() {
    var oid2 = Oid.fromString("13c633665257696a3800b0a39ff636b4593f918f");
    assert.notEqual(this.oid.cmp(oid2), 0);
  });

  it("can compare the first chunk of two identical oids", function() {
    assert.equal(this.oid.ncmp(this.oid, 5), 0);
  });

  it("can compare the first chunk of two different oids", function() {
    var oid2 = Oid.fromString("13c633665257696a3800b0a39ff636b4593f918f");
    assert.notEqual(this.oid.ncmp(oid2, 5), 0);
  });

  it("can check the equality of two identical oids", function() {
    assert(this.oid.equal(this.oid));
  });

  it("can check the equality of two different oids", function() {
    var oid2 = Oid.fromString("13c633665257696a3800b0a39ff636b4593f918f");
    assert(!this.oid.equal(oid2));
  });

  it("does not leak constructed Oid", function() {
    return leakTest(Oid, function() {
      return Promise.resolve(
        Oid.fromString("13c633665257696a3800b0a39ff636b4593f918f")
      );
    });
  });

  it("does not leak owned Oid", function() {
    return leakTest(Oid, function() {
      return NodeGit.Repository.open(local("../repos/workdir"))
        .then(function(repo) {
          return NodeGit.Commit.lookup(repo, oid);
        })
        .then(function(commit) {
          return commit.id();
        });
    });
  });
});
