var assert = require("assert");
var path = require("path");

describe("Revwalk", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Repository = require("../../lib/repository");
  var Revwalk = require("../../lib/revwalk");
  var Oid = require("../../lib/oid");

  beforeEach(function(done) {
    // These inits run extremely fast locally but time out on travis
    this.timeout(4000);
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;
      test.walker = repository.createRevWalk();

      return test.repository.getBranch("rev-walk").then(function(branch) {
        test.branch = branch;
        test.walker.push(test.branch.id());
        done();
      });
    });
  });

  it("can create a walker", function() {
    assert.ok(this.walker instanceof Revwalk);
  });

  it("can push an object", function() {
    var sha = this.branch.sha();

    return this.walker.next().then(function(commit) {
      assert.equal(sha, commit);
    });
  });

  it("can hide an object", function() {
    var test = this;

    return test.walker.next().then(function(commit) {
      return test.walker.next().then(function(commit) {
        return test.walker.next().then(function(commit) {
          return test.walker.next().then(function(commit) {
            assert.equal(commit.toString(),
              "b8a94aefb22d0534cc0e5acf533989c13d8725dc");
            test.walker = test.repository.createRevWalk();
            test.walker.push(test.branch.id());
            test.walker.hide(
              Oid.fromstr("b8a94aefb22d0534cc0e5acf533989c13d8725dc"));

            return test.walker.next().then(function(commit) {
              return test.walker.next().then(function(commit) {
                return test.walker.next().then(function(commit) {
                  assert.equal(commit.toString(),
                    "95f695136203a372751c19b6353aeb5ae32ea40e");
                  return test.walker.next().then(function(commit) {
                    assert.equal(commit, undefined);
                  });
                });
              });
            });
          });
        });
      });
    });
  });

  it("can simplify to first parent", function() {
    var test = this;

    test.walker.simplifyFirstParent();
    return test.walker.next().then(function(commit) {
      return test.walker.next().then(function(commit) {
        return test.walker.next().then(function(commit) {
          assert.equal(commit.toString(),
            "b8a94aefb22d0534cc0e5acf533989c13d8725dc");
        });
      });
    });
  });

  // This test requires forcing garbage collection, so mocha needs to be run via
  // node rather than npm, with a la `node --expose-gc [pathtohmoca] [testglob]`
  var testGC = (global.gc ? it : it.skip);

  testGC("doesnt segfault when accessing .author() twice", function(done) {
      this.timeout(10000);
      return Repository.open(reposPath).then(function(repository) {
        var walker = repository.createRevWalk();
        repository.getMaster().then(function(master) {
          var did = false;
          walker.walk(master, function(error, commit) {
            for (var i = 0; i < 1000; i++) {
              if (true) {
                commit.author().name();
                commit.author().email();
              }
              global.gc();
            }
            if (!did) {
              done();
              did = true;
            }
          });
        });
      });
    });

});
