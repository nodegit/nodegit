var NodeGit = require("../");
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
  var remoteCallbacks;

  if (options) {
    remoteCallbacks = options.remoteCallbacks;
    delete options.remoteCallbacks;
  }

  options = normalizeOptions(options, NodeGit.CloneOptions);

  if (remoteCallbacks) {
    options.remoteCallbacks =
      normalizeOptions(remoteCallbacks, NodeGit.RemoteCallbacks);
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
