var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var lookupWrapper = NodeGit.Utils.lookupWrapper;

var Remote = NodeGit.Remote;
var setCallbacks = Remote.prototype.setCallbacks;
var push = Remote.prototype.push;

/**
 * Retrieves the remote by name
 * @async
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

/**
 * Pushes to a remote
 *
 * @async
 * @param {Array} refSpecs The ref specs that should be pushed
 * @param {PushOptions} options Options for the checkout
 * @param {Signature} signature The identity to use for the reflog of the
 * updated references
 * @param {String} message The message to use for the update reflog messages
 * @return {Number} error code
 */
Remote.prototype.push = function(refSpecs, options, signature, message) {
  options = normalizeOptions(options, NodeGit.PushOptions);
  signature = signature || this.owner().defaultSignature();
  message = message || "Push to " + this.name();

  return push.call(this, refSpecs, options, signature, message);
};

module.exports = Remote;
