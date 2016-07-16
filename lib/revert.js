var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Revert = NodeGit.Revert;

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
var commit = Revert.commit;
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

  return commit.call(
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
