module.exports = function(NodeGit) {
  this.checkoutOpts = new NodeGit.CheckoutOptions();
  this.mainline = 0;
  this.mergeOpts = new NodeGit.MergeOptions();
};
