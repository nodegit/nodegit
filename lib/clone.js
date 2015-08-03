var NodeGit = require("../");
var shallowClone = require("./utils/shallow_clone");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Clone = NodeGit.Clone;
var clone = Clone.clone;

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
  var remoteCallbacks = {};
  var fetchOpts = {};

  if (options) {
    options = shallowClone(options);
    if (options.fetchOpts) {
      fetchOpts = shallowClone(options.fetchOpts);
    }
    delete options.fetchOpts;
  }

  options = normalizeOptions(options, NodeGit.CloneOptions);

  if (fetchOpts.callbacks) {
    remoteCallbacks = shallowClone(fetchOpts.callbacks);
    delete fetchOpts.callbacks;
  }

  fetchOpts = normalizeOptions(fetchOpts, NodeGit.FetchOptions);

  fetchOpts.callbacks =
    normalizeOptions(remoteCallbacks, NodeGit.RemoteCallbacks);

  if (options) {
    options.fetchOpts = fetchOpts;
  }

  // This is required to clean up after the clone to avoid file locking
  // issues in Windows and potentially other issues we don't know about.
  var freeRepository = function(repository) {
    repository.free();
  };

  // We want to provide a valid repository object, so reopen the repository
  // after clone and cleanup.
  var openRepository = function() {
    return NodeGit.Repository.open(local_path);
  };

  return clone.call(this, url, local_path, options)
    .then(freeRepository)
    .then(openRepository);
};
