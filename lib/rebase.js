var NodeGit = require("../");
var Rebase = NodeGit.Rebase;
var normalizeOptions = NodeGit.Utils.normalizeOptions;

// Override Rebase.prototype.finish to normalize opts
var finish = Rebase.prototype.finish;
Rebase.prototype.finish = function(signature, opts) {
  opts = normalizeOptions(opts || {}, NodeGit.RebaseOptions);
  return finish.call(this, signature, opts);
};

// Override Rebase.prototype.next to normalize opts and provide good defaults
var next = Rebase.prototype.next;
Rebase.prototype.next = function(checkoutOpts) {
  if (!checkoutOpts) {
    checkoutOpts = {
      checkoutStrategy: NodeGit.Checkout.STRATEGY.SAFE_CREATE
    };
  }

  checkoutOpts = normalizeOptions(checkoutOpts, NodeGit.CheckoutOptions);
  return next.call(this, checkoutOpts);
};
