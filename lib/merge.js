var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var Promise = require("nodegit-promise");

var Merge = NodeGit.Merge;
var mergeCommits = Merge.commits;
var mergeMerge = Merge.merge;

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

/**
 * Merge a commit into HEAD and writes the results to the working directory.
 *
 * @param {Repository} repo Repository that contains the given commits
 * @param {Commit} theirHead The annotated to merge into HEAD
 * @param {MergeOptions} [mergeOpts] The merge tree options (null for default)
 * @param {CheckoutOptions} [checkoutOpts] The checkout options
 *                                         (null for default)
 */
Merge.merge = function(repo, theirHead, mergeOpts, checkoutOpts) {
  mergeOpts = normalizeOptions(mergeOpts || {}, NodeGit.MergeOptions);
  checkoutOpts = normalizeOptions(checkoutOpts || {}, NodeGit.CheckoutOptions);

  // Even though git_merge takes an array of annotated_commits, it expects
  // exactly one to have been passed in or it will throw an error...  ¯\_(ツ)_/¯
  var theirHeads = [theirHead];

  return mergeMerge.call(this, repo, theirHeads, theirHeads.length,
    mergeOpts, checkoutOpts);
};
