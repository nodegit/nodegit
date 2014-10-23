var NodeGit = require("../");
var normalizeOptions = require("./util/normalize_options");

var Clone = NodeGit.Clone;
var clone = Clone.clone;

/**
 * Patch repository cloning to automatically coerce objects.
 *
 * @param url
 * @param local_path
 * @param options
 */
Clone.clone = function(url, local_path, options) {
  var remoteCallbacks;

  if (options) {
    remoteCallbacks = options.remoteCallbacks;
    delete options.remoteCallbacks;
  }

  options = normalizeOptions(options, NodeGit.CloneOptions);

  if (remoteCallbacks) {
    options.remoteCallbacks = normalizeOptions(remoteCallbacks,
      NodeGit.RemoteCallbacks);
  }

  return clone.call(this, url, local_path, options);
};

module.exports = Clone;
