var assert = require("assert");

describe.skip("Converting valuetypes in v8", function() {
  var nodegit = require("../..");

  describe("integer-booleans (1s and 0s)", function() {
    var vals = [0, 1, 2, true, false, {}, "radishes", null, undefined, "", []];

    it("can accept truthy/falsey values in setters", function() {
      vals.forEach(function (val) {
        var opts = { ignoreCertErrors: val };
        opts = new nodegit.CloneOptions(opts);
      });
    });

    it("can retrieve true/false values via getters", function() {
      vals.forEach(function (val) {
        var opts = { ignoreCertErrors: val };
        opts = new nodegit.CloneOptions(opts);
        assert.equal(opts.ignoreCertErrors, !!val);
      });
    });
  });
});
