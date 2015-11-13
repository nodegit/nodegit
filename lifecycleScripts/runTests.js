var spawn = require("child_process").spawn;
var Promise = require("nodegit-promise");
var path = require("path");
var objectAssign = require("object-assign");

var determineTarget = require("./determineTarget");

return determineTarget()
  .then(function(targetInfo) {
    var opts = {
      cwd: ".",
      maxBuffer: Number.MAX_VALUE,
      env: objectAssign({}, process.env),
      stdio: "inherit",
      stderr: "inherit",
    };
    var binaryName = "node";
    var args = ["--expose-gc", "test"];

    if (targetInfo.target === "electron" && process.platform !== "linux") {
      binaryName = path.resolve("node_modules", ".bin", "electron");

      if (process.platform === "win32") {
        binaryName += ".cmd";
      }

      opts.env.ATOM_SHELL_INTERNAL_RUN_AS_NODE = "1";
    }

    return new Promise(function(resolve, reject) {
      var task = spawn(binaryName, args, opts);
      task.on("exit", function(code) {
        if (code === 0) {
          resolve();
        } else {
          reject(code);
        }
        process.exit(code);
      });
    });
  });
