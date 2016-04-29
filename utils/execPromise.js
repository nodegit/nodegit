var promisify = require("promisify-node");
var cp = require('child_process');

module.exports = promisify(function(command, opts, callback) {
  return cp.exec(command, opts, callback);
});
