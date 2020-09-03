const { parentPort, workerData } = require("worker_threads");
const assert = require("assert");
const NodeGit = require("../../");

const { clonePath, url } = workerData;
const opts = {
    fetchOpts: {
      callbacks: {
        certificateCheck: () => 0
    }
  }
};

return NodeGit.Clone(url, clonePath, opts).then(repo => {
  assert.ok(repo instanceof NodeGit.Repository);
  parentPort.postMessage(true);
}).catch(() => {
  parentPort.postMessage(false);
});
