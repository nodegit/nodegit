var NodeGit = require("../../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var shallowClone = NodeGit.Utils.shallowClone;

/**
 * Normalize an object to match a struct.
 *
 * @param {String, Object} oid - The oid string or instance.
 * @return {Object} An Oid instance.
 */
function normalizeFetchOptions(options) {
  if (options instanceof NodeGit.FetchOptions) {
    return options;
  }

  var callbacks;
  var proxyOpts;

  if (options) {
    options = shallowClone(options);
    callbacks = options.callbacks;
    proxyOpts = options.proxyOpts;
    delete options.callbacks;
    delete options.proxyOpts;
  } else {
    options = {};
  }

  options = normalizeOptions(options, NodeGit.FetchOptions);

  if (callbacks) {
    options.callbacks =
      normalizeOptions(callbacks, NodeGit.RemoteCallbacks);
  }

  if (proxyOpts) {
    options.proxyOpts =
      normalizeOptions(proxyOpts, NodeGit.ProxyOptions);
  }
  return options;
}

NodeGit.Utils.normalizeFetchOptions = normalizeFetchOptions;
