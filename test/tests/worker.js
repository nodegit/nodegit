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
      it(`can kill worker thread while in use #${i}`, function(done) { // jshint ignore:line
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
              setTimeout(() => { worker.terminate(); }, 500);
              break;
            case "success":
              assert.fail();
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

    // NOTE: first try was to build a test measuring memory used, checking
    // that memory allocated by objects was being freed, but it was problematic
    // to obtain the memory freed by a different context (a worker) after the
    // context was gone, and the data in the tests wasn't consistent.
    // So instead this test checks that the count of objects created/destroyed
    // during the test match the count of objects being tracked by the
    // nodegit::Context, which will be destroyed on context shutdown. To check
    // that they are actually being freed can be done with a debugger/profiler.
    it("can track objects to free on context shutdown", function(done) {
      let testOk;
      const workerPath = local("../utils/worker_context_aware.js");
      const worker = new Worker(workerPath, {
        workerData: {
          clonePath,
          url: "https://github.com/nodegit/test.git"
        }
      });
      worker.on("message", (message) => {
        switch (message) {
          case "numbersMatch":
            testOk = true;
            worker.terminate();
            break;
          case "numbersDoNotMatch":
            testOk = false;
            worker.terminate();
            break;
          case "failure":
            assert.fail();
            break;
        }
      });
      worker.on("error", () => assert.fail());
      worker.on("exit", (code) => {
        if (code === 1 && testOk === true) {
          done();
        }
        else {
          assert.fail();
        }
      });
    });
  });
}
