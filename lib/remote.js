var NodeGit = require("../");
var normalizeOptions = require("./util/normalize_options");
var lookupWrapper = require("./util/lookupWrapper");

var Remote = NodeGit.Remote;
var setCallbacks = Remote.prototype.setCallbacks;

/**
 * Retrieves the remote by name
 * @param {Repository} repo The repo that the remote lives in
 * @param {String|Remote} name The remote to lookup
 * @param {Function} callback
 * @return {Remote}
 */
Remote.lookup = lookupWrapper(Remote);

Remote.prototype.setCallbacks = function(callbacks) {
  callbacks = normalizeOptions(callbacks, NodeGit.RemoteCallbacks);

  return setCallbacks.call(this, callbacks);
};

module.exports = Remote;
