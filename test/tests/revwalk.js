var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);

describe("Revwalk", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Revwalk = NodeGit.Revwalk;
  var Oid = NodeGit.Oid;

  var reposPath = local("../repos/workdir");

  // Set a reasonable timeout here now that our repository has grown.
  this.timeout(60000);

  beforeEach(function() {
    var test = this;
    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
        return test.repository.getBranchCommit("rev-walk");
      })
      .then(function(commit) {
        test.commit = commit;
      });
  });

  beforeEach(function() {
    this.walker = this.repository.createRevWalk();
    this.walker.push(this.commit.id());
  });

  it("can create a walker", function() {
    assert.ok(this.walker instanceof Revwalk);
  });

  it("can push an object", function() {
    var sha = this.commit.sha();

    return this.walker.next()
      .then(function(commit) {
        assert.equal(sha, commit);
      });
  });

  it("can hide an object", function() {
    var test = this;

    return next(test.walker, 4)
      .then(function(commit) {
        assert.equal(commit.toString(),
          "b8a94aefb22d0534cc0e5acf533989c13d8725dc");

        test.walker = test.repository.createRevWalk();
        test.walker.push(test.commit.id());
        test.walker.hide(
          Oid.fromString("b8a94aefb22d0534cc0e5acf533989c13d8725dc"));

        return next(test.walker, 3);
      })
      .then(function(commit) {
        assert.equal(commit.toString(),
          "95f695136203a372751c19b6353aeb5ae32ea40e");
          return next(test.walker, 1);
      })
      .then(function(commit) {
        assert.equal(commit, undefined);
      });
  });

  it("can simplify to first parent", function() {
    var test = this;

    test.walker.simplifyFirstParent();
    return next(test.walker, 3)
      .then(function(commit) {
        assert.equal(commit.toString(),
          "b8a94aefb22d0534cc0e5acf533989c13d8725dc");
      });
  });

  it("can get a specified number of commits", function() {
    var test = this;
    var storedCommits;
    return test.walker.getCommits()
      .then(function(commits) {
        assert.equal(commits.length, 10);
        storedCommits = commits;
        test.walker = test.repository.createRevWalk();
        test.walker.push(test.commit.id());

        return test.walker.getCommits(8);
      })
      .then(function(commits) {
        assert.equal(commits.length, 8);
        for (var i = 0; i < 8; i++) {
          assert.equal(commits[i].toString(), storedCommits[i].toString());
        }
      });
  });

  it("can get commits until you tell it not to", function() {
    var test = this;
    var magicSha = "b8a94aefb22d0534cc0e5acf533989c13d8725dc";

    function checkCommit(commit) {
      return commit.toString() != magicSha;
    }

    return test.walker.getCommitsUntil(checkCommit)
      .then(function(commits) {
        assert.equal(commits.length, 4);
        assert.equal(commits[commits.length-1].toString(), magicSha);
      });
  });

  // This test requires forcing garbage collection, so mocha needs to be run
  // via node rather than npm, with a la `node --expose-gc [pathtohmoca]
  // [testglob]`
  var testGC = global.gc ? it : it.skip;

  testGC("doesnt segfault when accessing .author() twice", function(done) {
    Repository.open(reposPath).then(function(repository) {
      var walker = repository.createRevWalk();

      repository.getMasterCommit().then(function(firstCommitOnMaster) {
        walker.walk(firstCommitOnMaster, function(err, commit) {
          if (!err && !commit) {
            return done();
          }

          for (var i = 0; i < 500; i++) {
            commit.author().name();
            commit.author().email();

            if ( i % 250 === 0) {
              global.gc();
            }
          }
        });
      });
    });
  });

  function next(walker, count) {
    var promise = null;
    for (var i = 0; i < count; i++) {
      if (!promise) {
        promise = walker.next();
      }
      else {
        promise = promise.then(getNext);
      }
    }
    return promise;

    function getNext() {
      return walker.next();
    }
  }
});
