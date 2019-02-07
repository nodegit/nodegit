var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

var garbageCollect = require("../utils/garbage_collect.js");
var exec = require("../../utils/execPromise");

describe("Signature", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Signature = NodeGit.Signature;

  var reposPath = local("../repos/workdir");

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

  it("can get a default signature when no user name is set", function() {
    var savedUserName;
    var savedUserEmail;

    var cleanUp = () => {
      return exec("git config --global user.name \"" + savedUserName + "\"")
      .then(() => {
        exec("git config --global user.email \"" + savedUserEmail + "\"");
      });
    };

    return exec("git config --global user.name")
    .then((userName) => {
      savedUserName = userName.trim();

      return exec("git config --global user.email");
    })
    .then((userEmail) => {
      savedUserEmail = userEmail.trim();

      return exec("git config --global --unset user.name");
    })
    .then(() => {
      return exec("git config --global --unset user.email");
    })
    .then(() => {
      return Repository.open(reposPath);
    })
    .then((repo) => {
      return repo.defaultSignature();
    })
    .then((sig) => {
      assert.equal(sig.name(), "unknown");
      assert.equal(sig.email(), "unknown@example.com");
    })
    .then(cleanUp)
    .catch((e) => {
      return cleanUp()
      .then(function() {
        return Promise.reject(e);
      });
    });
  });

  it("duplicates time", function() {
    garbageCollect();
    var Time = NodeGit.Time;
    var startSelfFreeingCount = Time.getSelfFreeingInstanceCount();
    var startNonSelfFreeingCount =
      Time.getNonSelfFreeingConstructedCount();
    var time = Signature.now(name, email).when();

    garbageCollect();
    var endSelfFreeingCount = Time.getSelfFreeingInstanceCount();
    var endNonSelfFreeingCount = Time.getNonSelfFreeingConstructedCount();
    // we should get one duplicated, self-freeing time
    assert.equal(startSelfFreeingCount + 1, endSelfFreeingCount);
    assert.equal(startNonSelfFreeingCount, endNonSelfFreeingCount);

    time = null;
    garbageCollect();
    endSelfFreeingCount = Time.getSelfFreeingInstanceCount();
    // the self-freeing time should get freed
    assert.equal(startSelfFreeingCount, endSelfFreeingCount);
  });

  it("toString does not provide a timestamp by default", function () {
    const signature = Signature.create(
      "Shaggy Rogers",
      "shaggy@mystery.com",
      987654321,
      90
    );

    assert.equal(signature.toString(), "Shaggy Rogers <shaggy@mystery.com>");
  });

  it("toString provides the correct timestamp when requested", function() {
    const signature = Signature.create(
      "Shaggy Rogers",
      "shaggy@mystery.com",
      987654321,
      90
    );

    assert.equal(
      signature.toString(true),
      "Shaggy Rogers <shaggy@mystery.com> 987654321 +0130"
    );
  });
});
