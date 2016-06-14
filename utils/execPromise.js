var cp = require('child_process');

// We have to manually promisify this because at this is required in lifecycle
// methods and we are not guaranteed that any 3rd party packages are installed
// at this point
module.exports = function(command, opts) {
  return new Promise(function(resolve, reject) {
    return cp.exec(command, opts, function(err, result) {
      if (err) {
        reject(err);
      }
      else {
        resolve(result);
      }
    });
  });
};
