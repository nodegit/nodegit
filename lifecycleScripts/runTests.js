var spawn = require("child_process").spawn;
var Promise = require("nodegit-promise");
var path = require("path");

var determineTarget = require("./determineTarget");

return determineTarget()
  .then(function(targetInfo) {
    var opts = {
      cwd: ".",
      maxBuffer: Number.MAX_VALUE,
      env: process.env
    };
    var binaryName = "node";
    var args = ["--expose-gc", "test"];
    if (targetInfo.target === "electron") {
      binaryName = path.resolve("node_modules", ".bin", "electron");

      if (process.platform === "win32") {
        binaryName += ".cmd";
      }

      opts.env.ATOM_SHELL_INTERNAL_RUN_AS_NODE = "1";
    }

    return new Promise(function(resolve, reject) {
      var task = spawn(binaryName, args, opts);
      task.stdout.pipe(process.stdout);
      task.stderr.pipe(process.stderr);
      task.on("exit", function(code) {
        if (code === 0) {
          resolve();
        } else {
          reject(code);
        }
      });
    });
  });
