var promisify = require("promisify-node");

module.exports = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});
