const {
  isMainThread,
  parentPort,
  workerData
} = require("worker_threads");
const garbageCollect = require("./garbage_collect.js");
const assert = require("assert");
const NodeGit = require("../../");
const { promisify } = require("util");

if (isMainThread) {
  throw new Error("Must be run via worker thread");
}

const { clonePath, url } = workerData;
const opts = {
    fetchOpts: {
      callbacks: {
        certificateCheck: () => 0
    }
  }
};

let repository;
const oid = "fce88902e66c72b5b93e75bdb5ae717038b221f6";

return NodeGit.Clone(url, clonePath, opts)
.then((_repository) => {
  repository = _repository;
  assert.ok(repository instanceof NodeGit.Repository);
  return repository.getCommit(oid);
}).then((commit) => {
  assert.ok(commit instanceof NodeGit.Commit);
  var historyCount = 0;
  var history = commit.history();

  history.on("commit", function(commit) {
    // Number of commits is known to be higher than 200
    if (++historyCount == 200) {
      // Tracked objects must work too when the Garbage Collector is triggered
      garbageCollect();

      // Count total of objects left after being created/destroyed
      const freeingCount =
        NodeGit.Cert.getNonSelfFreeingConstructedCount() +
        NodeGit.Repository.getSelfFreeingInstanceCount() +
        NodeGit.Commit.getSelfFreeingInstanceCount() +
        NodeGit.Oid.getSelfFreeingInstanceCount() +
        NodeGit.Revwalk.getSelfFreeingInstanceCount();

      const numberOfTrackedObjects = NodeGit.getNumberOfTrackedObjects();

      if (freeingCount === numberOfTrackedObjects) {
        parentPort.postMessage("numbersMatch");
      }
      else {
        parentPort.postMessage("numbersDoNotMatch");
      }
    }
  });
  
  history.on("end", function(commits) {
    // Test should not get this far
    parentPort.postMessage("failure");
  });

  history.on("error", function(err) {
    assert.ok(false);
  });

  history.start();
  
  return promisify(setTimeout)(50000);
}).catch(() => parentPort.postMessage("failure"));