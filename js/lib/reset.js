var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Reset = NodeGit.Reset;

/**
 * Look up a refs's commit.
 *
 * @async
 * @param {Repository} repo Repository where to perform the reset operation.
 * @param {Object} target The committish which content will be used to reset the
 *                        content of the index.
 * @param {Strarray} pathspecs List of pathspecs to operate on.
 *
 * @return {Number} 0 on success or an error code
 */
var defaultFn = Reset.default;
Reset.default = function(repo, target, pathspecs) {
  return defaultFn.call(this, repo, target, pathspecs);
};

/**
 * Look up a refs's commit.
 *
 * @async
 * @param {Repository} repo Repository where to perform the reset operation.
 *
 * @param {Object} target Committish to which the Head should be moved to. This
 *                        object must belong to the given `repo` and can either
 *                        be a git_commit or a git_tag. When a git_tag is being
 *                        passed, it should be dereferencable to a git_commit
 *                        which oid will be used as the target of the branch.
 * @param {Number} resetType Kind of reset operation to perform.
 *
 * @param {CheckoutOptions} opts Checkout options to be used for a HARD reset.
 *                               The checkout_strategy field will be overridden
 *                               (based on reset_type). This parameter can be
 *                               used to propagate notify and progress
 *                               callbacks.
 *
 * @param {String|Ref} name Ref name, e.g. "master", "refs/heads/master"
 *                          or Branch Ref
 *
 * @return {Number} 0 on success or an error code
 */
var reset = Reset.reset;
Reset.reset = function(repo, target, resetType, opts) {
  opts = normalizeOptions(opts, NodeGit.CheckoutOptions);

  return reset.call(this, repo, target, resetType, opts);
};

module.exports = Reset;
