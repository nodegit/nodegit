var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Reset = NodeGit.Reset;
var defaultFn = Reset.default;

Reset.default = function(repo, target, pathspecs) {
  return defaultFn.call(this, repo, target, pathspecs);
};

var reset = Reset.reset;
Reset.reset = function(repo, target, resetType, opts, signature, logMessage) {
  opts = normalizeOptions(opts, NodeGit.CheckoutOptions);

  return reset.call(this, repo, target, resetType, opts, signature, logMessage);
};

module.exports = Reset;
