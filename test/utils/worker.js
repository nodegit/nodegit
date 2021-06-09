const {
  isMainThread,
  parentPort,
  workerData
} = require("worker_threads");
const assert = require("assert");
const NodeGit = require("../../");
const { promisify } = require("util");

if (isMainThread) {
  throw new Error("Must be run via worker thread");
}

parentPort.postMessage("init");

const { clonePath, url } = workerData;
const opts = {
    fetchOpts: {
      callbacks: {
        certificateCheck: () => 0
    }
  }
};

let repository;
return NodeGit.Clone(url, clonePath, opts).then((_repository) => {
  repository = _repository;
  assert.ok(repository instanceof NodeGit.Repository);
  return repository.index();
}).then((index) => {
  assert.ok(index instanceof NodeGit.Index);
  return repository.getRemoteNames();
}).then((remotes) => {
  assert.ok(Array.isArray(remotes));
  return repository.getCurrentBranch();
}).then((branch) => {
  assert.ok(branch instanceof NodeGit.Reference);
  parentPort.postMessage("success");
  return promisify(setTimeout)(5000);
}).catch(() => parentPort.postMessage("failure"));
