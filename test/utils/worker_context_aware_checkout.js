const {
  isMainThread,
  parentPort,
  workerData
} = require("worker_threads");
const garbageCollect = require("./garbage_collect.js");
const assert = require("assert");
const NodeGit = require("../../");
const loopingCheckoutHead = require("./loop_checkout.js");
const { promisify } = require("util");

if (isMainThread) {
  throw new Error("Must be run via worker thread");
}

const { clonePath, url } = workerData;
const cloneOpts = {
    fetchOpts: {
      callbacks: {
        certificateCheck: () => 0
    }
  }
};

let repository;
let filterName = "psuedo_filter";
let applyCallbackResult = 1;

return NodeGit.Clone(url, clonePath, cloneOpts)
.then(function(_repository) {
  repository = _repository;
  assert.ok(repository instanceof NodeGit.Repository);
  return NodeGit.FilterRegistry.register(filterName, {
    apply: function() {
      applyCallbackResult = 0;
    },
    check: function() {
      return NodeGit.Error.CODE.OK;
    }
  }, 0);
})
.then(function(result) {
  assert.strictEqual(result, NodeGit.Error.CODE.OK);
  return loopingCheckoutHead(clonePath, repository, 10);
}).then(function() {
  assert.strictEqual(applyCallbackResult, 0);
  // Tracked objects must work too when the Garbage Collector is triggered
  garbageCollect();

  // Count total of objects left after being created/destroyed
  const freeingCount =
    NodeGit.Cert.getNonSelfFreeingConstructedCount() +
    NodeGit.FilterSource.getNonSelfFreeingConstructedCount() +
    NodeGit.Buf.getNonSelfFreeingConstructedCount() +
    NodeGit.Repository.getSelfFreeingInstanceCount();

  const numberOfTrackedObjects = NodeGit.getNumberOfTrackedObjects();

  if (freeingCount === numberOfTrackedObjects) {
    parentPort.postMessage("numbersMatch");
  }
  else {
    parentPort.postMessage("numbersDoNotMatch");
  }
  return promisify(setTimeout)(50000);
}).catch((err) => parentPort.postMessage("failure"));