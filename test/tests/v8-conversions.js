var assert = require("assert");

describe.skip("Converting valuetypes in v8", function() {
  var nodegit = require("../..");

  it("can accept truthy values for int-bools in setters", function() {
    var vals = [0, 1, 2, true, false, {}, "radishes", null, undefined, "", []];

    vals.forEach(function (val) {
      var opts = { ignoreCertErrors: val };
      opts = new nodegit.CloneOptions(opts);
      assert.equal(opts.ignoreCertErrors, !!val);
    });
  });
});
