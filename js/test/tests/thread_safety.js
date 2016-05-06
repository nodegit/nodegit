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
        return repo.refreshIndex();
      })
      .then(function(index) {
        test.index = index;
      });
  });

  it("can enable and disable thread safety", function() {
    var originalValue = NodeGit.getThreadSafetyStatus();

    NodeGit.enableThreadSafety();
    assert.equal(NodeGit.THREAD_SAFETY.ENABLED,
      NodeGit.getThreadSafetyStatus());

    NodeGit.setThreadSafetyStatus(NodeGit.THREAD_SAFETY.ENABLED_FOR_ASYNC_ONLY);
    assert.equal(NodeGit.THREAD_SAFETY.ENABLED_FOR_ASYNC_ONLY,
      NodeGit.getThreadSafetyStatus());

    NodeGit.setThreadSafetyStatus(NodeGit.THREAD_SAFETY.DISABLED);
    assert.equal(NodeGit.THREAD_SAFETY.DISABLED,
      NodeGit.getThreadSafetyStatus());

    NodeGit.setThreadSafetyStatus(originalValue);
  });

  it("can lock something and cleanup mutex", function() {
    var diagnostics = NodeGit.getThreadSafetyDiagnostics();
    var originalCount = diagnostics.storedMutexesCount;
    // call a sync method to guarantee that it stores a mutex,
    // and that it will clean up the mutex in a garbage collection cycle
    this.repository.headDetached();

    diagnostics = NodeGit.getThreadSafetyDiagnostics();
    switch(NodeGit.getThreadSafetyStatus()) {
      case NodeGit.THREAD_SAFETY.ENABLED:
        // this is a fairly vague test - it just tests that something
        // had a mutex created for it at some point (i.e., the thread safety
        // code is not completely dead)
        assert.ok(diagnostics.storedMutexesCount > 0);
        break;
      case NodeGit.THREAD_SAFETY.ENABLED_FOR_ASYNC_ONLY:
        assert.equal(originalCount, diagnostics.storedMutexesCount);
        break;

      case NodeGit.THREAD_SAFETY.DISABLED:
        assert.equal(0, diagnostics.storedMutexesCount);
    }
  });
});
