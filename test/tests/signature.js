var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
describe("Signature", function() {

  var Signature = require(local("../../lib/signature"));
  var name = "Bob Gnarley";
  var email = "gnarlee@bob.net";
  var arbitraryDate = 123456789;
  var timezoneOffset = 60;

  it("can be created at an arbitrary time", function() {
    var create = Signature.create;
    var signature = create(name, email, arbitraryDate, timezoneOffset);

    assert.equal(signature.name(), name);
    assert.equal(signature.email(), email);
    assert.equal(signature.when().time(), arbitraryDate);
    assert.equal(signature.when().offset(), 60);

  });

  it("can be created now", function() {

    var signature = Signature.now(name, email);
    var now = new Date();
    var when = signature.when();
    var diff = Math.abs(when.time() - now/1000);
    assert.equal(signature.name(), name);
    assert.equal(signature.email(), email);
    assert(diff <= 1);

    // libgit2 does its timezone offsets backwards from javascript
    assert.equal(when.offset(), -now.getTimezoneOffset());
  });
});
