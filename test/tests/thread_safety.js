var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("ThreadSafety", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;

  var reposPath = local("../repos/workdir");

  beforeEach(function() {
    var test = this;

    return Repository.open(reposPath)
      .then(function(repo) {
        test.repository = repo;
        return repo.openIndex();
      })
      .then(function(index) {
        test.index = index;
      });
  });

  it("can enable and disable thread safety", function() {
    var originalValue = NodeGit.isThreadSafetyEnabled();

    NodeGit.enableThreadSafety();
    assert.equal(true, NodeGit.isThreadSafetyEnabled());

    NodeGit.disableThreadSafety();
    assert.equal(false, NodeGit.isThreadSafetyEnabled());

    // flip the switch again, to make sure we test all transitions
    // (we could have started with thread safety enabled)
    NodeGit.enableThreadSafety();
    assert.equal(true, NodeGit.isThreadSafetyEnabled());

    if (originalValue) {
      NodeGit.enableThreadSafety();
    } else {
      NodeGit.disableThreadSafety();
    }
  });

  it("can lock something and cleanup mutex", function() {
    // call a sync method to guarantee that it stores a mutex,
    // and that it will clean up the mutex in a garbage collection cycle
    this.repository.headDetached();

    var diagnostics = NodeGit.getThreadSafetyDiagnostics();
    if (NodeGit.isThreadSafetyEnabled()) {
      // this is a fairly vague test - it just tests that something
      // had a mutex created for it at some point (i.e., the thread safety
      // code is not completely dead)
      assert.ok(diagnostics.storedMutexesCount > 0);
      // now test that GC cleans up mutexes
      global.gc();
      diagnostics = NodeGit.getThreadSafetyDiagnostics();
      assert.equal(0, diagnostics.storedMutexesCount);
    } else {
      assert.equal(0, diagnostics.storedMutexesCount);
    }
  });
});
