var NodeGit = require("../");
var normalizeOptions = require("./util/normalize_options");

var Remote = NodeGit.Remote;
var setCallbacks = Remote.prototype.setCallbacks;

Remote.prototype.setCallbacks = function(callbacks) {
  callbacks = normalizeOptions(callbacks, NodeGit.RemoteCallbacks);

  return setCallbacks.call(this, callbacks);
};

module.exports = Remote;
