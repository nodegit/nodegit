var assert = require("assert");

var garbageCollect = require("./garbage_collect");

function leakTest(Type, getInstance) {
  garbageCollect();
  var startSelfFreeingCount = Type.getSelfFreeingInstanceCount();
  var startNonSelfFreeingCount = Type.getNonSelfFreeingConstructedCount();

  var resolve;
  var promise = new Promise(function(_resolve) { resolve = _resolve; });

  getInstance()
    .then(function() {
      var selfFreeingCount = Type.getSelfFreeingInstanceCount();
      assert.equal(startSelfFreeingCount + 1, selfFreeingCount);
      // get out of this promise chain to help GC get rid of the commit
      setTimeout(resolve, 0);
    });

  return promise
    .then(function() {
      garbageCollect();
      var endSelfFreeingCount = Type.getSelfFreeingInstanceCount();
      var endNonSelfFreeingCount = Type.getNonSelfFreeingConstructedCount();
      // any new self-freeing commits should have been freed
      assert.equal(startSelfFreeingCount, endSelfFreeingCount);
      // no new non-self-freeing commits should have been constructed
      assert.equal(startNonSelfFreeingCount, endNonSelfFreeingCount);
    });
}

module.exports = leakTest;
