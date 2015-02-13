var NodeGit = require("../");
var normalizeOptions = require("./util/normalize_options");

var Checkout = NodeGit.Checkout;
var head = Checkout.head;
var tree = Checkout.tree;

/**
* Patch head checkout to automatically coerce objects.
*
* @param url
* @param options
*/
Checkout.head = function(url, options) {
  options = normalizeOptions(options, NodeGit.CheckoutOptions);

  return head.call(this, url, options);
};

/**
* Patch tree checkout to automatically coerce objects.
*
* @param repo
* @param treeish
* @param options
*/
Checkout.tree = function(repo, treeish, options) {
  options = normalizeOptions(options, NodeGit.CheckoutOptions);

  return tree.call(this, repo, treeish, options);
};

module.exports = Checkout;
