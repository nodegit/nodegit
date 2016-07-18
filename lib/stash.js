var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var shallowClone = NodeGit.Utils.shallowClone;
var Stash = NodeGit.Stash;

var _apply = Stash.apply;
var _foreach = Stash.foreach;
var _pop = Stash.pop;

Stash.apply = function(repo, index, options) {
  var checkoutOptions;

  if (options) {
    options = shallowClone(options);
    checkoutOptions = options.checkoutOptions;
    delete options.checkoutOptions;
  } else {
    options = {};
  }

  options = normalizeOptions(options, NodeGit.StashApplyOptions);

  if (checkoutOptions) {
    options.checkoutOptions =
      normalizeOptions(checkoutOptions, NodeGit.CheckoutOptions);
  }

  return _apply(repo, index, options);
};

// Override Stash.foreach to eliminate the need to pass null payload
Stash.foreach = function(repo, callback) {
  function wrappedCallback(index, message, oid) {
    // We need to copy the OID since libgit2 types are getting cleaned up
    // incorrectly right now in callbacks

    return callback(index, message, oid.copy());
  }

  return _foreach(repo, wrappedCallback, null);
};

Stash.pop = function(repo, index, options) {
  var checkoutOptions;

  if (options) {
    options = shallowClone(options);
    checkoutOptions = options.checkoutOptions;
    delete options.checkoutOptions;
  } else {
    options = {};
  }

  options = normalizeOptions(options, NodeGit.StashApplyOptions);

  if (checkoutOptions) {
    options.checkoutOptions =
      normalizeOptions(checkoutOptions, NodeGit.CheckoutOptions);
  }

  return _pop(repo, index, options);
};
