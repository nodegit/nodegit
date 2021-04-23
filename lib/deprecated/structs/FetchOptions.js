module.exports = function(NodeGit) {
  this.callbacks = new NodeGit.RemoteCallbacks();
  this.downloadTags = 0;
  this.proxyOpts = new NodeGit.ProxyOptions();
  this.prune = 0;
  this.updateFetchhead = 1;
};
