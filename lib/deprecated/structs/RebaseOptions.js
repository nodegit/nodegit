module.exports = function(NodeGit) {
  this.checkoutOptions = new NodeGit.CheckoutOptions();
  this.inmemory = 0;
  this.mergeOptions = new NodeGit.MergeOptions();
  this.quiet = 0;
};
