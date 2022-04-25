var NodeGit = require("../");

var Graph = NodeGit.Graph;

var _reachableFromAny = Graph.reachableFromAny;

/**
 * Determine if a commit is reachable from any of a list of commits by following parent edges.
 * @param {repository} the repository where the commits exist
 * @param {commit} a previously loaded commit
 * @param {descendant_array} oids of the commits
 */
Graph.reachableFromAny = function(repository, commit, descendant_array) {
  return _reachableFromAny(repository, commit, descendant_array, descendant_array.length);
};
