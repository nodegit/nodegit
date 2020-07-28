var util = require("util");
var NodeGit = require("../");
var normalizeFetchOptions = NodeGit.Utils.normalizeFetchOptions;
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var lookupWrapper = NodeGit.Utils.lookupWrapper;
var shallowClone = NodeGit.Utils.shallowClone;

var Remote = NodeGit.Remote;
var _connect = Remote.prototype.connect;
var _createWithOpts = Remote.createWithOpts;
var _disconnect = Remote.prototype.disconnect;
var _download = Remote.prototype.download;
var _fetch = Remote.prototype.fetch;
var _push = Remote.prototype.push;
var _updateTips = Remote.prototype.updateTips;
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

  return _connect.call(this, direction, callbacks, proxyOpts, customHeaders)
    .then(() => {
      // Save options on the remote object. If we don't do this,
      // the options may be cleaned up and cause a segfault
      // when Remote.prototype.connect is called.
      Object.defineProperties(this, {
        callbacks: {
          configurable: true,
          value: callbacks,
          writable: false
        },
        proxyOpts: {
          configurable: true,
          value: proxyOpts,
          writable: false
        },
        customHeaders: {
          configurable: true,
          value: customHeaders,
          writable: false
        }
      });
    });
};

Remote.createWithOpts = function(url, options) {
  return _createWithOpts(url, normalizeOptions(
    options, NodeGit.RemoteCreateOptions));
};

Remote.prototype.disconnect = function() {
  return _disconnect.call(this)
    .then(() => {
      // Release the options
      Object.defineProperties(this, {
        callbacks: {
          configurable: true,
          value: undefined,
          writable: false
        },
        proxyOpts: {
          configurable: true,
          value: undefined,
          writable: false
        },
        customHeaders: {
          configurable: true,
          value: undefined,
          writable: false
        }
      });
    });
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
 * Update the tips to the new state
 * @param {RemoteCallbacks} callbacks The callback functions for the connection
 * @param {boolean} updateFetchhead whether to write to FETCH_HEAD. Pass true
 *                                  to behave like git.
 * @param {boolean} downloadTags what the behaviour for downloading tags is
 *                               for this fetch. This is ignored for push.
 *                               This must be the same value passed to
 *                               Remote.prototype.download
 * @param {string} reflogMessage The message to insert into the reflogs. If
 *                               null and fetching, the default is "fetch ",
 *                               where is the name of the remote (or its url,
 *                               for in-memory remotes). This parameter is
 *                               ignored when pushing.
 */
Remote.prototype.updateTips = function(
  callbacks,
  updateFetchhead,
  downloadTags,
  reflogMessage
) {
  if (callbacks) {
    callbacks = normalizeOptions(callbacks, NodeGit.RemoteCallbacks);
  }

  return _updateTips.call(
    this,
    callbacks,
    updateFetchhead,
    downloadTags,
    reflogMessage
  );
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

NodeGit.Remote.COMPLETION_TYPE = {};
var DEPRECATED_STATES = {
  COMPLETION_DOWNLOAD: "DOWNLOAD",
  COMPLETION_INDEXING: "INDEXING",
  COMPLETION_ERROR: "ERROR"
};

Object.keys(DEPRECATED_STATES).forEach((key) => {
  const newKey = DEPRECATED_STATES[key];
  Object.defineProperty(NodeGit.Remote.COMPLETION_TYPE, key, {
    get: util.deprecate(
      () => NodeGit.Remote.COMPLETION[newKey],
      `Use NodeGit.Remote.COMPLETION.${newKey} instead of ` +
      `NodeGit.Remote.COMPLETION_TYPE.${key}.`
    )
  });
});
