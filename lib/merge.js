var NodeGit = require("../");
var normalizeOptions = require("./util/normalize_options");

var Merge = NodeGit.Merge;
var mergeCommits = Merge.commits;

/**
 * Merge 2 commits together and create an new index that can
 * be used to create a merge commit.
 *
 * @param repo Repository that contains the given commits
 * @param ourCommit The commit that reflects the destination tree
 * @oaram theirCommit The commit to merge into ourCommit
 * @param options The merge tree options (null for default)
 */
Merge.commits = function(repo, ourCommit, theirCommit, options) {
  options = normalizeOptions(options, NodeGit.MergeOptions);

  return mergeCommits.call(this, repo, ourCommit, theirCommit, options);
};

module.exports = Merge;
