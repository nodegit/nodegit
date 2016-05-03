var cp = require("child_process");

module.exports = function gitExecutableLocation() {
  return new Promise(function(resolve, reject) {
    var cmd;

    if (process.platform === "win32") {
      cmd = "where git";
    }
    else {
      cmd = "which git";
    }

    cp.exec(cmd, function(err, stdout, stderr) {
      if (err) {
        reject(err, stderr);
      }
      else {
        resolve(stdout);
      }
    });
  });
};
