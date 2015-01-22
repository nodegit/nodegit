var NodeGit = require("../");
var normalizeOptions = require("./util/normalize_options");

var Checkout = NodeGit.Checkout;
var head = Checkout.head;

/**
* Patch head checkout to automatically coerce objects.
*
* @param repo
* @param options
*/
Checkout.head = function(url, options) {
  options = normalizeOptions(options, NodeGit.CheckoutOptions);

  return head.call(this, url, options);
};


module.exports = Checkout;
