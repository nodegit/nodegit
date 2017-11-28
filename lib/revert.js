var NodeGit = require("../");
var shallowClone = NodeGit.Utils.shallowClone;
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Revert = NodeGit.Revert;
var _commit = Revert.commit;
var _revert = Revert.revert;

/**
 * Reverts the given commit against the given "our" commit, producing an index
 * that reflects the result of the revert.
 *
 * @async
 * @param {Repository} repo the repository that contains the given commits.
 * @param {Commit} revert_commit the commit to revert
 * @param {Commit} our_commit the commit to revert against (e.g. HEAD)
 * @param {Number} mainline the parent of the revert commit, if it is a merge
 * @param {MergeOptions} merge_options the merge options (or null for defaults)
 *
 * @return {Index} the index result
 */
Revert.commit = function(
  repo,
  revert_commit,
  our_commit,
  mainline,
  merge_options,
  callback
)
{
  merge_options = normalizeOptions(merge_options, NodeGit.MergeOptions);

  return _commit.call(
    this,
    repo,
    revert_commit,
    our_commit,
    mainline,
    merge_options
  )
    .then(function(result) {
      if (typeof callback === "function") {
        callback(null, result);
      }

      return result;
    }, callback);
};

/**
 * Reverts the given commit, producing changes in the index and
 * working directory.
 * 
 * @async
 * @param {Repository} repo the repository to perform the revert in
 * @param {Commit} commit the commit to revert
 * @param {RevertOptions} revert_options the revert options
 *                                       (or null for defaults)
 */
Revert.revert = function(repo, commit, revertOpts) {
  var mergeOpts;
  var checkoutOpts;

  if (revertOpts) {
    revertOpts = shallowClone(revertOpts);
    mergeOpts = revertOpts.mergeOpts;
    checkoutOpts = revertOpts.checkoutOpts;
    delete revertOpts.mergeOpts;
    delete revertOpts.checkoutOpts;
  }

  revertOpts = normalizeOptions(revertOpts, NodeGit.RevertOptions);

  if (revertOpts) {
    revertOpts.mergeOpts =
      normalizeOptions(mergeOpts, NodeGit.MergeOptions);
  }

  if (checkoutOpts) {
    revertOpts.checkoutOpts =
      normalizeOptions(checkoutOpts, NodeGit.CheckoutOptions);
  }

  return _revert.call(this, repo, commit, revertOpts);
};
