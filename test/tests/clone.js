var assert = require("assert");
var path = require("path");
var Promise = require("promise");
var promisify = require("promisify-node");
var rimraf = promisify("rimraf");

describe("Clone", function() {
  var http = path.resolve("test/repos/http");
  var https = path.resolve("test/repos/https");
  var ssh = path.resolve("test/repos/ssh");
  var file = path.resolve("test/repos/file");

  var Repository = require("../../lib/repository");
  var Clone = require("../../lib/clone");

  before(function() {
    return Promise.all([
      rimraf(http),
      rimraf(https),
      rimraf(ssh),
      rimraf(file),
    ]);
  });

  it.skip("can clone with http", function() {
    var url = "http://github.com/nodegit/test.git";

    return Clone.clone(url, http, null).then(function(repository) {
      assert.ok(repository instanceof Repository);
    });
  });

  it.skip("can clone with https", function() {
    var url = "https://github.com/nodegit/test.git";

    return Clone.clone(url, https, null).then(function(repository) {
      assert.ok(repository instanceof Repository);
    });
  });

  it.skip("can clone with ssh", function() {
    var url = "git@github.com:nodegit/test.git";

    return Clone.clone(url, ssh, null).then(function(repository) {
      assert.ok(repository instanceof Repository);
    });
  });

  it("can clone with filesystem", function() {
    var url = "file://" + path.resolve("test/repos/empty");

    return Clone.clone(url, file, null).then(function(repository) {
      assert.ok(repository instanceof Repository);
    });
  });
});
