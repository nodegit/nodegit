var assert = require("assert");
var path = require("path");

describe("Revwalk", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Repository = require("../../lib/repository");
  var Revwalk = require("../../lib/revwalk");
  var Oid = require("../../lib/oid");

  // Set a reasonable timeout here now that our repository has grown.
  this.timeout(60000);

  before(function(done) {
    var test = this;
    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;
      return test.repository.getBranchCommit("rev-walk").then(function(commit) {
        test.commit = commit;
        done();
      });
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

    return this.walker.next().then(function(commit) {
      assert.equal(sha, commit);
    });
  });

  it("can hide an object", function() {
    var test = this;

    return test.walker.next().then(function(commit) {
      return test.walker.next();
    }).then(function() {
      return test.walker.next();
    }).then(function() {
      return test.walker.next();
    }).then(function(commit) {
      assert.equal(commit.toString(),
        "b8a94aefb22d0534cc0e5acf533989c13d8725dc");
      test.walker = test.repository.createRevWalk();
      test.walker.push(test.commit.id());
      test.walker.hide(
        Oid.fromString("b8a94aefb22d0534cc0e5acf533989c13d8725dc"));

      return test.walker.next();
    }).then(function() {
      return test.walker.next();
    }).then(function() {
      return test.walker.next();
    }).then(function(commit) {
      assert.equal(commit.toString(),
        "95f695136203a372751c19b6353aeb5ae32ea40e");
      return test.walker.next();
    }).then(function(commit) {
      assert.equal(commit, undefined);
    });
  });

  it("can simplify to first parent", function() {
    var test = this;

    test.walker.simplifyFirstParent();
    return test.walker.next().then(function() {
      return test.walker.next();
    }).then(function() {
      return test.walker.next();
    }).then(function(commit) {
      assert.equal(commit.toString(),
        "b8a94aefb22d0534cc0e5acf533989c13d8725dc");
    });
  });

  it.only("can get a specified number of commits", function() {
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
});
