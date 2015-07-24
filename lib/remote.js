var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var lookupWrapper = NodeGit.Utils.lookupWrapper;
var shallowClone = require("./utils/shallow_clone");

var Remote = NodeGit.Remote;
var connect = Remote.prototype.connect;
var download = Remote.prototype.download;
var fetch = Remote.prototype.fetch;
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

Remote.prototype.connect = function(direction, callbacks) {
  callbacks = normalizeOptions(callbacks, NodeGit.RemoteCallbacks);

  return connect.call(this, direction, callbacks);
};


Remote.prototype.download = function(refspecs, opts) {
  var callbacks;

  if (opts) {
    opts = shallowClone(opts);
    callbacks = opts.callbacks;
    delete opts.callbacks;
  } else {
    opts = {};
  }

  opts = normalizeOptions(opts, NodeGit.FetchOptions);

  if (callbacks) {
    opts.callbacks =
      normalizeOptions(callbacks, NodeGit.RemoteCallbacks);
  }

  return download.call(this, refspecs, opts);
};

Remote.prototype.fetch = function(refspecs, opts, reflog_message) {
  var callbacks;

  if (opts) {
    opts = shallowClone(opts);
    callbacks = opts.callbacks;
    delete opts.callbacks;
  } else {
    opts = {};
  }

  opts = normalizeOptions(opts, NodeGit.FetchOptions);

  if (callbacks) {
    opts.callbacks =
      normalizeOptions(callbacks, NodeGit.RemoteCallbacks);
  }

  return fetch.call(this, refspecs, opts, reflog_message);
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
Remote.prototype.push = function(refSpecs, opts) {
  var callbacks;
  if (opts) {
    opts = shallowClone(opts);
    callbacks = opts.callbacks;
    delete opts.callbacks;
  } else {
    opts = {};
  }

  opts = normalizeOptions(opts, NodeGit.PushOptions);

  if (callbacks) {
    opts.callbacks =
      normalizeOptions(callbacks, NodeGit.RemoteCallbacks);
  }

  return push.call(this, refSpecs, opts);
};

module.exports = Remote;
