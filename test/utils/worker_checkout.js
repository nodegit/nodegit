const {
  isMainThread,
  parentPort,
  workerData
} = require("worker_threads");
const assert = require("assert");
const NodeGit = require("../../");
const loopingCheckoutHead = require("./loop_checkout.js");

if (isMainThread) {
  throw new Error("Must be run via worker thread");
}

parentPort.postMessage("init");

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
  return loopingCheckoutHead(clonePath, repository, 0);
}).then(function() {
  assert.strictEqual(applyCallbackResult, 0);
  parentPort.postMessage("success");
})
.catch((err) => { 
  parentPort.postMessage("failure");
});