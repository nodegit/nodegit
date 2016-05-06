var NodeGit = require("../");
var shallowClone = require("./utils/shallow_clone");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Cherrypick = NodeGit.Cherrypick;
var _cherrypick = Cherrypick.cherrypick;
var _commit = Cherrypick.commit;

/**
* Cherrypick a commit and, changing the index and working directory
*
* @async
* @param {Repository}         repo      The repo to checkout head
* @param {Commit}             commit    The commit to cherrypick
* @param {CherrypickOptions}  [options] Options for the cherrypick
* @return {int} 0 on success, -1 on failure
*/
Cherrypick.cherrypick = function(repo, commit, options) {
  var mergeOpts;
  var checkoutOpts;

  if (options) {
    options = shallowClone(options);
    mergeOpts = options.mergeOpts;
    checkoutOpts = options.checkoutOpts;
    delete options.mergeOpts;
    delete options.checkoutOpts;
  }

  options = normalizeOptions(options, NodeGit.CherrypickOptions);

  if (mergeOpts) {
    options.mergeOpts =
      normalizeOptions(mergeOpts, NodeGit.MergeOptions);
  }

  if (checkoutOpts) {
    options.checkoutOpts =
      normalizeOptions(checkoutOpts, NodeGit.CheckoutOptions);
  }

  return _cherrypick.call(this, repo, commit, options);
};

/**
* Cherrypicks the given commit against "our" commit, producing an index that
* reflects the result of the cherrypick. The index is not backed by a repo.
*
* @async
* @param {Repository}   repo              The repo to cherrypick commits
* @param {Commit}       cherrypick_commit The commit to cherrypick
* @param {Commit}       our_commit        The commit to revert against
* @param {int}          mainline          The parent of the revert commit (1 or
*                                         2) if it's a merge, 0 otherwise
* @param {MergeOptions} [merge_options]   Merge options for the cherrypick
* @return {int}   0 on success, -1 on failure
*/
Cherrypick.commit = function(
    repo,
    cherrypick_commit,
    our_commit,
    mainline,
    merge_options) {
  merge_options = normalizeOptions(merge_options, NodeGit.MergeOptions);

  return _commit.call(
    this,
    repo,
    cherrypick_commit,
    our_commit,
    mainline,
    merge_options);
};
