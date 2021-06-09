const path = require("path");
const assert = require("assert");
const fse = require("fs-extra");
const local = path.join.bind(path, __dirname);

let Worker;

try {
  Worker = require("worker_threads").Worker;
} catch (e) {}

if (Worker) {
  describe("Worker", function() {
    const clonePath = local("../repos/clone");

    // Set a reasonable timeout here now that our repository has grown.
    this.timeout(30000);

    beforeEach(function() {
      return fse.remove(clonePath).catch(function(err) {
        console.log(err);

        throw err;
      });
    });

    it("can perform basic functionality via worker thread", function(done) {
      const workerPath = local("../utils/worker.js");
      const worker = new Worker(workerPath, {
        workerData: {
          clonePath,
          url: "https://github.com/nodegit/test.git"
        }
      });
      worker.on("message", (message) => {
        switch (message) {
          case "init":
            break;
          case "success":
            done();
            break;
          case "failure":
            assert.fail();
            break;
        }
      });
      worker.on("error", () => assert.fail());
      worker.on("exit", (code) => {
        if (code !== 0) {
          assert.fail();
        }
      });
    });

    for (let i = 0; i < 5; ++i) {
      it.only(`can kill worker thread while in use #${i}`, function(done) { // jshint ignore:line
        const workerPath = local("../utils/worker.js");
        const worker = new Worker(workerPath, {
          workerData: {
            clonePath,
            url: "https://github.com/nodegit/test.git"
          }
        });
        worker.on("message", (message) => {
          switch (message) {
            case "init":
              break;
            case "success":
              worker.terminate();
              break;
            case "failure":
              assert.fail();
              break;
          }
        });
        worker.on("error", () => assert.fail());
        worker.on("exit", (code) => {
          if (code === 1) {
            done();
          } else {
            assert.fail();
          }
        });
      });
    }
  });
}
