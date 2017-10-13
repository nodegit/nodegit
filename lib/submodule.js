var NodeGit = require("../");
var normalizeFetchOptions = NodeGit.Utils.normalizeFetchOptions;
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var shallowClone = NodeGit.Utils.shallowClone;

var Submodule = NodeGit.Submodule;

var _foreach = Submodule.foreach;
var _update = Submodule.prototype.update;

// Override Submodule.foreach to eliminate the need to pass null payload
Submodule.foreach = function(repo, callback) {
  return _foreach(repo, callback, null);
};

/**
 * Updates a submodule
 *
 * @async
 * @param {Number} init Setting this to 1 will initialize submodule
 *                      before updating
 * @param {SubmoduleUpdateOptions} options Submodule update settings
 * @return {Number} 0 on success, any non-zero return value from a callback
 */
Submodule.prototype.update = function(init, options) {
  var fetchOpts;
  var checkoutOpts;

  if (options) {
    options = shallowClone(options);
    fetchOpts = options.fetchOpts;
    checkoutOpts = options.checkoutOpts;
    delete options.fetchOpts;
    delete options.checkoutOpts;
  }
  
  options = normalizeOptions(options, NodeGit.SubmoduleUpdateOptions);

  if (fetchOpts) {
    options.fetchOpts = normalizeFetchOptions(fetchOpts);
  }

  if (checkoutOpts) {
    options.checkoutOpts = normalizeOptions(
      checkoutOpts,
      NodeGit.CheckoutOptions
    );
  }

  return _update.call(this, init, options);
};
