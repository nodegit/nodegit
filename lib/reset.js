var NodeGit = require("../");

var Reset = NodeGit.Reset;
var _default = Reset.default;
var _reset = Reset.reset;

/**
 * Look up a refs's commit.
 *
 * @param {Repository}   repo       Repository where to perform the reset operation.
 * @param {Commit | Tag} target     The committish which content will be used to reset the content of the index.
 * @param {Strarray}     pathspecs  List of pathspecs to operate on.
 * @returns {number} 0. On success or an error code.
 * @async
 */
Reset.default = function (repo, target, pathspecs) {
  return _default.call(this, repo, target, pathspecs);
};

/**
 * Reset a repository's current HEAD to the specified target.
 *
 * @param {Repository}      repo       Repository where to perform the reset operation.
 * @param {Commit | Tag}    target     Committish to which the Head should be moved to.
 *                                     This object must belong to the given `repo` and can either be a git_commit or a
 *                                     git_tag. When a git_tag is being passed, it should be dereferencable to a
 *                                     git_commit which oid will be used as the target of the branch.
 * @param {number}          resetType  Kind of reset operation to perform.
 * @param {CheckoutOptions} opts       Checkout options to be used for a HARD reset.
 *                                     The checkout_strategy field will be overridden (based on reset_type). This
 *                                     parameter can be used to propagate notify and progress callbacks.
 * @returns {number} 0. On success or an error code.
 * @async
 */
Reset.reset = function (repo, target, resetType, opts) {
  if (repo !== target.repo) {
    // this is the same that is performed on libgit2's side
    // https://github.com/nodegit/libgit2/blob/8d89e409616831b7b30a5ca7b89354957137b65e/src/reset.c#L120-L124
    throw new Error("Repository and target commit's repository does not match");
  }
  return _reset.call(this, repo, target, resetType, opts);
};
