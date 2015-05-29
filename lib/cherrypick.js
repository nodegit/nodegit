var NodeGit = require("../");
var shallowClone = require("./utils/shallow_clone");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Cherrypick = NodeGit.Cherrypick;
var _cherrypick = Cherrypick.cherrypick;

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
