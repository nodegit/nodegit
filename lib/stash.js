var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var shallowClone = require("./utils/shallow_clone");
var Stash = NodeGit.Stash;

// Override Stash.foreach to eliminate the need to pass null payload
var foreach = Stash.foreach;
Stash.foreach = function(repo, callback) {
  function wrappedCallback(index, message, oid) {
    // We need to copy the OID since libgit2 types are getting cleaned up
    // incorrectly right now in callbacks

    return callback(index, message, oid.copy());
  }

  return foreach(repo, wrappedCallback, null);
};

var sApply = Stash.apply;
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
      normalizeOptions(checkoutOptions, NodeGit.checkoutOptions);
  }

  return sApply(repo, index, options);
};

var pop = Stash.pop;
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

  return pop(repo, index, options);
};
