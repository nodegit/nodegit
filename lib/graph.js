var NodeGit = require("../");

var Graph = NodeGit.Graph;

var _reachableFromAny = Graph.reachableFromAny;

/**
 * Determine if a commit is reachable from any of a list of commits by following parent edges.
 *
 * @param {repository}       the   Repository where the commits exist.
 * @param {commit}           a     Previously loaded commit.
 * @param {descendant_array} oids  Of the commits.
 */
Graph.reachableFromAny = function (repository, commit, descendant_array) {
  return _reachableFromAny(repository, commit, descendant_array, descendant_array.length);
};
