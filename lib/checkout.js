var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Checkout = NodeGit.Checkout;
var head = Checkout.head;
var tree = Checkout.tree;

/**
* Patch head checkout to automatically coerce objects.
*
* @async
* @param {Repository} repo The repo to checkout head
* @param {CheckoutOptions} [options] Options for the checkout
* @return {Void} checkout complete
*/
Checkout.head = function(url, options) {
  options = normalizeOptions(options, NodeGit.CheckoutOptions);

  return head.call(this, url, options);
};

/**
* Patch tree checkout to automatically coerce objects.
*
* @async
* @param {Repository} repo
* @param {Oid|Tree|Commit|Reference} treeish
* @param {CheckoutOptions} [options]
* @return {Void} checkout complete
*/
Checkout.tree = function(repo, treeish, options) {
  options = normalizeOptions(options, NodeGit.CheckoutOptions);

  return tree.call(this, repo, treeish, options);
};
