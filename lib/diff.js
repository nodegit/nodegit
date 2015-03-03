var NodeGit = require("../");
var Diff = NodeGit.Diff;
var ConvenientPatch = NodeGit.ConvenientPatch;
var normalizeOptions = NodeGit.Utils.normalizeOptions;
var Patch = NodeGit.Patch;


/**
 * Retrieve patches in this difflist
 *
 * @return {[ConvenientPatch]} an array of ConvenientPatches
 */
Diff.prototype.patches = function() {
  var size = this.numDeltas();
  var result = [];

  for (var i = 0; i < size; i++) {
    result.push(new ConvenientPatch(this.getDelta(i), Patch.fromDiff(this, i)));
  }

  return result;
};

// Override Diff.indexToWorkdir to normalize opts
var indexToWorkdir = Diff.indexToWorkdir;
Diff.indexToWorkdir = function(repo, index, opts) {
  opts = normalizeOptions(opts, NodeGit.DiffOptions);
  return indexToWorkdir(repo, index, opts);
};

// Override Diff.treeToIndex to normalize opts
var treeToIndex = Diff.treeToIndex;
Diff.treeToIndex = function(repo, tree, index, opts) {
  opts = normalizeOptions(opts, NodeGit.DiffOptions);
  return treeToIndex(repo, tree, index, opts);
};

// Override Diff.treeToTree to normalize opts
var treeToTree = Diff.treeToTree;
Diff.treeToTree = function(repo, from_tree, to_tree, opts) {
  opts = normalizeOptions(opts, NodeGit.DiffOptions);
  return treeToTree(repo, from_tree, to_tree, opts);
};

// Override Diff.treeToWorkdir to normalize opts
var treeToWorkdir = Diff.treeToWorkdir;
Diff.treeToWorkdir = function(repo, tree, opts) {
  opts = normalizeOptions(opts, NodeGit.DiffOptions);
  return treeToWorkdir(repo, tree, opts);
};

// Override Diff.treeToWorkdir to normalize opts
var treeToWorkdirWithIndex = Diff.treeToWorkdirWithIndex;
Diff.treeToWorkdirWithIndex = function(repo, tree, opts) {
  opts = normalizeOptions(opts, NodeGit.DiffOptions);
  return treeToWorkdirWithIndex(repo, tree, opts);
};

// Override Diff.findSimilar to normalize opts
var findSimilar = Diff.prototype.findSimilar;
Diff.prototype.findSimilar = function(opts) {
  opts = normalizeOptions(opts, NodeGit.DiffFindOptions);
  return findSimilar.call(this, opts);
};
