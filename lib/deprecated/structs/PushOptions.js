module.exports = function(NodeGit) {
  this.callbacks = new NodeGit.RemoteCallbacks();
  this.pbParallelism = 1;
  this.proxyOpts = new NodeGit.ProxyOptions();
};
