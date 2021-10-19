var cp = require('child_process');

// We have to manually promisify this because at this is required in lifecycle
// methods and we are not guaranteed that any 3rd party packages are installed
// at this point
module.exports = function(command, opts, extraOpts = {}) {
  return new Promise(function(resolve, reject) {
    const childProcess = cp.exec(command, opts, function(err, result) {
      if (err) {
        reject(err);
      }
      else {
        resolve(result);
      }
    });

    if (extraOpts.pipeOutput) {
      childProcess.stdout.pipe(process.stdout);
      childProcess.stderr.pipe(process.stderr);
    }

    return childProcess;
  });
};
