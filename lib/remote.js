var NodeGit = require("../");
var normalizeFetchOptions = NodeGit.Utils.normalizeFetchOptions;
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var lookupWrapper = NodeGit.Utils.lookupWrapper;
var shallowClone = NodeGit.Utils.shallowClone;

var Remote = NodeGit.Remote;
var _connect = Remote.prototype.connect;
var _download = Remote.prototype.download;
var _fetch = Remote.prototype.fetch;
var _push = Remote.prototype.push;
var _upload = Remote.prototype.upload;

/**
 * Retrieves the remote by name
 * @async
 * @param {Repository} repo The repo that the remote lives in
 * @param {String|Remote} name The remote to lookup
 * @param {Function} callback
 * @return {Remote}
 */
Remote.lookup = lookupWrapper(Remote);

/**
 * Connects to a remote
 *
 * @async
 * @param {Enums.DIRECTION} direction The direction for the connection
 * @param {RemoteCallbacks} callbacks The callback functions for the connection
 * @param {ProxyOptions} proxyOpts Proxy settings
 * @param {Array<string>} customHeaders extra HTTP headers to use
 * @param {Function} callback
 * @return {Number} error code
 */
Remote.prototype.connect = function(
  direction,
  callbacks,
  proxyOpts,
  customHeaders
) {
  callbacks = normalizeOptions(callbacks || {}, NodeGit.RemoteCallbacks);
  proxyOpts = normalizeOptions(proxyOpts || {}, NodeGit.ProxyOptions);
  customHeaders = customHeaders || [];

  return _connect.call(this, direction, callbacks, proxyOpts, customHeaders);
};

/**
 * Connects to a remote
 *
 * @async
 * @param {Array} refSpecs The ref specs that should be pushed
 * @param {FetchOptions} opts The fetch options for download, contains callbacks
 * @param {Function} callback
 * @return {Number} error code
 */
Remote.prototype.download = function(refspecs, opts) {
  return _download
    .call(this, refspecs, normalizeFetchOptions(opts));
};

/**
 * Connects to a remote
 *
 * @async
 * @param {Array} refSpecs The ref specs that should be pushed
 * @param {FetchOptions} opts The fetch options for download, contains callbacks
 * @param {String} message The message to use for the update reflog messages
 * @param {Function} callback
 * @return {Number} error code
 */
Remote.prototype.fetch = function(refspecs, opts, reflog_message) {
  return _fetch
    .call(this, refspecs, normalizeFetchOptions(opts), reflog_message);
};

/**
 * Pushes to a remote
 *
 * @async
 * @param {Array} refSpecs The ref specs that should be pushed
 * @param {PushOptions} options Options for the checkout
 * @param {Function} callback
 * @return {Number} error code
 */
Remote.prototype.push = function(refSpecs, opts) {
  var callbacks;
  var proxyOpts;

  if (opts) {
    opts = shallowClone(opts);
    callbacks = opts.callbacks;
    proxyOpts = opts.proxyOpts;
    delete opts.callbacks;
    delete opts.proxyOpts;
  } else {
    opts = {};
  }

  opts = normalizeOptions(opts, NodeGit.PushOptions);

  if (callbacks) {
    opts.callbacks =
      normalizeOptions(callbacks, NodeGit.RemoteCallbacks);
  }

  if (proxyOpts) {
    opts.proxyOpts =
      normalizeOptions(proxyOpts, NodeGit.ProxyOptions);
  }

  return _push.call(this, refSpecs, opts);
};

/**
 * Lists advertised references from a remote. You must connect to the remote
 * before using referenceList.
 *
 * @async
 * @return {Promise<Array<RemoteHead>>} a list of the remote heads the remote
 *                                      had available at the last established
 *                                      connection.
 *
 */
Remote.prototype.referenceList = Remote.prototype.referenceList;

/**
 * Connects to a remote
 *
 * @async
 * @param {Array} refSpecs The ref specs that should be pushed
 * @param {FetchOptions} opts The fetch options for download, contains callbacks
 * @param {String} message The message to use for the update reflog messages
 * @param {Function} callback
 * @return {Number} error code
 */
Remote.prototype.fetch = function(refspecs, opts, reflog_message) {
  return _fetch
    .call(this, refspecs, normalizeFetchOptions(opts), reflog_message);
};

/**
 * Pushes to a remote
 *
 * @async
 * @param {Array} refSpecs The ref specs that should be pushed
 * @param {PushOptions} options Options for the checkout
 * @param {Function} callback
 * @return {Number} error code
 */
Remote.prototype.upload = function(refSpecs, opts) {
  var callbacks;
  var proxyOpts;

  if (opts) {
    opts = shallowClone(opts);
    callbacks = opts.callbacks;
    proxyOpts = opts.proxyOpts;
    delete opts.callbacks;
    delete opts.proxyOpts;
  } else {
    opts = {};
  }

  opts = normalizeOptions(opts, NodeGit.PushOptions);

  if (callbacks) {
    opts.callbacks =
      normalizeOptions(callbacks, NodeGit.RemoteCallbacks);
  }

  if (proxyOpts) {
    opts.proxyOpts =
      normalizeOptions(proxyOpts, NodeGit.ProxyOptions);
  }

  return _upload.call(this, refSpecs, opts);
};
