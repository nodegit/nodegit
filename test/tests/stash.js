var assert = require("assert");
var path = require("path");
var promisify = require("thenify-all");
var Promise = require("bluebird");
var fse = promisify(require("fs-extra"), ["writeFile", "readFile"]);
var local = path.join.bind(path, __dirname);

describe("Stash", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Stash = NodeGit.Stash;

  var reposPath = local("../repos/workdir");

  before(function() {
    var test = this;
    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      });
  });

  it("gets no stashes on clean working directory", function() {
    var stashes = [];
    var stashCb = function(index, message, oid) {
      stashes.push({index: index, message: message, oid: oid});
    };

    return Stash.foreach(this.repository, stashCb)
      .then(function() {
        assert.equal(stashes.length, 0);
      });
  });

  it("can save and drop a stash", function() {
    var fileName = "README.md";
    var fileContent = "Cha-cha-cha-chaaaaaangessssss";
    var repo = this.repository;
    var filePath = path.join(repo.workdir(), fileName);
    var oldContent;
    var stashes = [];
    var stashOid;
    var stashMessage = "stash test";

    return fse.readFile(filePath)
      .then(function(content) {
        oldContent = content;
        return fse.writeFile(filePath, fileContent);
      })
      .then(function() {
        return Stash.save(repo, repo.defaultSignature(), stashMessage, 0);
      })
      .then(function(oid) {
        stashOid = oid;
        var stashCb = function(index, message, oid) {
          stashes.push({index: index, message: message, oid: oid});
        };

        return Stash.foreach(repo, stashCb);
      })
      .then(function() {
        assert.equal(stashes.length, 1);
        assert.equal(stashes[0].index, 0);
        assert.equal(stashes[0].message, "On master: " + stashMessage);
        assert.equal(stashes[0].oid.toString(), stashOid.toString());

        return Stash.drop(repo, 0);
      })
      .then(function () {
        stashes = [];
        var stashCb = function(index, message, oid) {
          stashes.push({index: index, message: message, oid: oid});
        };

        return Stash.foreach(repo, stashCb);
      })
      .then(function() {
        assert.equal(stashes.length, 0);
      })
      .catch(function(e) {
        return fse.writeFile(filePath, oldContent)
          .then(function() {
            return Promise.reject(e);
          });
      });
  });
});
