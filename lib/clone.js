var NodeGit = require("../");
var shallowClone = NodeGit.Utils.shallowClone;
var normalizeFetchOptions = NodeGit.Utils.normalizeFetchOptions;
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Clone = NodeGit.Clone;
var _clone = Clone.clone;

/**
 * Patch repository cloning to automatically coerce objects.
 *
 * @async
 * @param {String} url url of the repository
 * @param {String} local_path local path to store repository
 * @param {CloneOptions} [options]
 * @return {Repository} repo
 */
Clone.clone = function(url, local_path, options) {
  var fetchOpts = normalizeFetchOptions(options && options.fetchOpts);

  if (options) {
    options = shallowClone(options);
    delete options.fetchOpts;
  }

  options = normalizeOptions(options, NodeGit.CloneOptions);

  if (options) {
    options.fetchOpts = fetchOpts;
  }

  return _clone.call(this, url, local_path, options);
};
