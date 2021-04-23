module.exports = function(NodeGit) {
  this.bare = 0;
  this.checkoutOpts = new NodeGit.CheckoutOptions();
  this.fetchOpts = new NodeGit.FetchOptions();
  this.local = 0;
};
