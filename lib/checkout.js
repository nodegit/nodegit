var NodeGit = require("../");
var normalizeOptions = NodeGit.Utils.normalizeOptions;

var Checkout = NodeGit.Checkout;
var _head = Checkout.head;
var _index = Checkout.index;
var _tree = Checkout.tree;

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

  return _head.call(this, url, options);
};

/**
* Patch index checkout to automatically coerce objects.
*
* @async
* @param {Repository} repo The repo to checkout an index
* @param {Index} The index to checkout
* @param {CheckoutOptions} [options] Options for the checkout
* @return {Void} checkout complete
*/
Checkout.index = function(repo, index, options) {
  options = normalizeOptions(options, NodeGit.CheckoutOptions);

  return _index.call(this, repo, index, options);
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

  return _tree.call(this, repo, treeish, options);
};
