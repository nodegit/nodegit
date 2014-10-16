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
  //var opts = normalizeOptions(options, NodeGit.CloneOptions);
  //opts = normalizeOptions(options.remote_callbacks, NodeGit.RemoteCallbacks);

  var callbacks = new NodeGit.RemoteCallbacks();
  console.log(callbacks);
  //return clone.call(this, url, local_path, opts);
};

module.exports = Clone;
