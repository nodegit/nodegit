var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var Blame = NodeGit.Blame;

var _file = Blame.file;

/**
 * Retrieve the blame of a file
 *
 * @param {Repository} repo that contains the file
 * @param {String} path to the file to get the blame of
 * @param {BlameOptions} [options] Options for the blame
 */
Blame.file = function(repo, path, options) {
  options = normalizeOptions(options, NodeGit.BlameOptions);

  return _file.call(this, repo, path, options);
};
