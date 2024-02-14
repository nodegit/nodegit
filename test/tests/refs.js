var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

var exec = require("../../utils/execPromise");

describe("Reference", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Reference = NodeGit.Reference;
  var Reflog = NodeGit.Reflog;

  var reposPath = local("../repos/workdir");
  var refName = "refs/heads/master";

  before(function() {
    var test = this;

    return exec("git reset --hard origin/master", {cwd: reposPath})
      .then(function() {
        return Repository.open(reposPath);
      })
      .then(function(repository) {
        test.repository = repository;

        return repository.getReference(refName);
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

  it("can determine if the reference is not symbolic", function() {
    assert.ok(this.reference.isConcrete());
  });

  it("can check that reference is valid", function() {
    assert.ok(this.reference.isValid());
  });

  it("can return refName when casting toString", function() {
    assert.equal(this.reference.toString(), refName);
  });

  it("can compare two identical references", function() {
    assert.equal(this.reference.cmp(this.reference), 0);
  });

  it("can compare two different references", function() {
    var ref = this.reference;

    return this.repository.getReference("checkout-test")
      .then(function(otherRef) {
        assert.notEqual(ref.cmp(otherRef), 0);
      });
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

  it("can rename a reference", function() {
    var newRefName = "refs/heads/chasta-boran";
    var ref = this.reference;
    var repo = this.repository;
    var reflogMessage = "reflog message";
    var refExistsMessage = "Renamed ref still exists";

    return repo.getReference(newRefName)
      .then(function() {
        // The new ref name should not exist yet
        throw new Error(refExistsMessage);
      })
      .catch(function(err) {
        // Should throw an error explaining that the ref
        // does not exist
        assert.ok(err.message.includes(newRefName));
        return ref.rename(newRefName, 0, reflogMessage);
      })
      .then(function(reference) {
        // The ref should be renamed at this point
        assert.equal(reference.name(), newRefName);
        return repo.getReference(refName);
      })
      .then(function() {
        // The original ref name should not be found
        throw new Error(refExistsMessage);
      })
      .catch(function(err) {
        assert.ok(err.message.includes(refName));
        return Reflog.read(repo, newRefName);
      })
      .then(function(reflog) {
        var refEntryMessage = reflog
          .entryByIndex(0)
          .message();
        // The reflog should have the message passed to
        // the rename
        assert.equal(refEntryMessage, reflogMessage);
        return repo.getReference(newRefName);
      })
      .then(function(newRef) {
        // Set the ref name back to `master`
        return newRef.rename(refName, 0, "another reflog message");
      });
  });
});
