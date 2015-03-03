var assert = require("assert");
var path = require("path");
var promisify = require("promisify-node");
var local = path.join.bind(path, __dirname);

// Have to wrap exec, since it has a weird callback signature.
var exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});

describe("Reference", function() {
  var NodeGit = require(local("../../"));
  var Repository = NodeGit.Repository;
  var Reference = NodeGit.Reference;

  var reposPath = local("../repos/workdir");

  before(function() {
    var test = this;

    return exec("git reset --hard origin/master", {cwd: reposPath})
      .then(function() {
        return Repository.open(reposPath);
      })
      .then(function(repository) {
        test.repository = repository;

        return repository.getReference("refs/heads/master");
      })
      .then(function(reference) {
        test.reference = reference;
      });
  });

  it("can look up a reference", function() {
    assert.ok(this.reference instanceof Reference);
  });

  it("can determine if the reference is symbolic", function() {
    assert.equal(this.reference.isSymbolic(), false);
  });

  it("will return undefined looking up the symbolic target if not symbolic",
    function() {
      assert(this.reference.symbolicTarget() === undefined);
    });

  it("can look up the HEAD sha", function() {
    return Reference.nameToId(this.repository, "HEAD")
      .then(function(oid) {
        var sha = oid.allocfmt();
        assert.equal(sha, "32789a79e71fbc9e04d3eff7425e1771eb595150");
      });
  });
});
