var NodeGit = require("../");
var Patch = require("./patch");
var ConvenientPatch = require("./convenient_patch");

var Diff = NodeGit.Diff;

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

module.exports = Diff;
