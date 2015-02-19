var NodeGit = require("../");
var normalizeOptions = require("./util/normalize_options");

var Reset = NodeGit.Reset;

var defaultFn = Reset.default;
Reset.default = function(repo, target, pathspecs) {
  return defaultFn.call(this, repo, target, pathspecs);
};

var reset = Reset.reset;
Reset.reset = function( repo,
                        target,
                        resetType,
                        checkoutOpts,
                        signature,
                        logMessage) {
  checkoutOpts = normalizeOptions(checkoutOpts, NodeGit.CheckoutOptions);

  return reset.call(
    this,
    repo,
    target,
    resetType,
    checkoutOpts,
    signature,
    logMessage);
};

module.exports = Reset;
