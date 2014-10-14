var assert = require("assert");
var path = require("path");

describe("Revwalk", function() {
  var reposPath = path.resolve("test/repos/workdir/.git");

  var Repository = require("../../lib/repository");
  var Revwalk = require("../../lib/revwalk");
  var Oid = require("../../lib/oid");

  before(function() {
    var test = this;

    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;
      test.walker = repository.createRevWalk();

      return test.repository.getMaster().then(function(master) {
        test.master = master;
        test.walker.push(test.master.id());
      });
    });
  });

  it("can create a walker", function() {
    assert.ok(this.walker instanceof Revwalk);
  });

  it("can push an object", function() {
    var sha = this.master.sha();

    return this.walker.next().then(function(commit) {
      assert.equal(sha, commit);
    });
  });

  it.skip("can hide an object", function() {
    var test = this;

    this.walker.hide(Oid.fromstr("a03e044fcb45c654d4e15a4e495a6a0c6e632854"));

    return test.walker.next().then(function(commit) {
      return test.walker.next().then(function(commit) {
        assert.equal(commit, "1efa3354299ede235f90880383176fb5d48aaa89");
      });
    });
  });

  it.skip("can simplify to first parent", function() {
    var test = this;

    test.walker.simplifyFirstParent();

    return test.walker.next().then(function(commit) {
      return test.walker.next().then(function(commit) {
        assert.equal(commit, "231c550f3ec28874b4c426fc9eebad9a742e1332");
      });
    });
  });

  it("doesnt segfault when accessing commit.author() twice", function(done) {
    if (!global.gc) {
      assert(global.gc, 'manual garbage collection is not enabled')
    }
    this.timeout(10000);

    function wait(time) {
      var then = time + new Date().getTime();
      while (then > 0 + new Date().getTime()) { }
    }

    try {
      {

        return Repository.open(reposPath).then(function(repository) {
          try {
            var walker = repository.createRevWalk();
            repository.getMaster().then(function(master) {
              try {
                walker.walk(master, function(error, commit) {
                  commit.author().name();
                  commit.author().email();
                  global.gc();
                });
              }
              catch( e) {
                done(e);
              }

            });
          }
          catch(e) {
            done(e);
          }
        });
      }
      done();
    }
    catch(e) {
      done(e);
    }
  });

});
