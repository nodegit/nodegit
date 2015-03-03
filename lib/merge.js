var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var Promise = require("nodegit-promise");

var Merge = NodeGit.Merge;
var mergeCommits = Merge.commits;

/**
 * Merge 2 commits together and create an new index that can
 * be used to create a merge commit.
 *
 * @param {Repository} repo Repository that contains the given commits
 * @param {Commit} ourCommit The commit that reflects the destination tree
 * @param {Commit} theirCommit The commit to merge into ourCommit
 * @param {MergeOptions} [options] The merge tree options (null for default)
 */
Merge.commits = function(repo, ourCommit, theirCommit, options) {
  options = normalizeOptions(options, NodeGit.MergeOptions);

  return Promise.all([
    repo.getCommit(ourCommit),
    repo.getCommit(theirCommit)
  ]).then(function(commits) {
    return mergeCommits.call(this, repo, commits[0], commits[1], options);
  });
};
