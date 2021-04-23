module.exports = function(NodeGit) {
  this.allowFetch = 1;
  this.checkoutOpts = new NodeGit.CheckoutOptions();
  this.fetchOpts = new NodeGit.FetchOptions();
};
