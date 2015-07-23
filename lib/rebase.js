var NodeGit = require("../");
var Rebase = NodeGit.Rebase;
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var shallowClone = require("./utils/shallow_clone");

// Override Rebase.prototype.finish to normalize opts
var finish = Rebase.prototype.finish;
Rebase.prototype.finish = function(signature, opts) {
  opts = normalizeOptions(opts || {}, NodeGit.RebaseOptions);
  return finish.call(this, signature, opts);
};

var init = Rebase.init;
Rebase.init = function(repository, branch, upstream, onto, options) {
  var checkoutOptions;

  if (options) {
    options = shallowClone(options);
    checkoutOptions = options.checkoutOptions;
    delete options.checkoutOptions;

    options = normalizeOptions(options, NodeGit.RebaseOptions);
  } else {
    options = normalizeOptions({}, NodeGit.RebaseOptions);
    checkoutOptions = {
      checkoutStrategy: NodeGit.Checkout.STRATEGY.FORCE
    };
  }

  if (checkoutOptions) {
    options.checkoutOptions =
      normalizeOptions(checkoutOptions, NodeGit.CheckoutOptions);
  }

  return init(repository, branch, upstream, onto, options);
};

var rebaseOpen = Rebase.open;
Rebase.open = function(repository, options) {
  var checkoutOptions;

  if (options) {
    options = shallowClone(options);
    checkoutOptions = options.checkoutOptions;
    delete options.checkoutOptions;

    options = normalizeOptions(options, NodeGit.RebaseOptions);
  } else {
    options = normalizeOptions({}, NodeGit.RebaseOptions);
    checkoutOptions = {
      checkoutStrategy: NodeGit.Checkout.STRATEGY.SAFE
    };
  }

  if (checkoutOptions) {
    options.checkoutOptions =
      normalizeOptions(checkoutOptions, NodeGit.CheckoutOptions);
  }

  return rebaseOpen(repository, options);
};
