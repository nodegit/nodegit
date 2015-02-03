var assert = require("assert");
var path = require("path");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));

describe("Status", function() {
  var reposPath = path.resolve("test/repos/workdir");

  var Status = require("../../lib/status");
  var Repository = require("../../lib/repository");

  before(function(done) {
    var test = this;
    return Repository.open(reposPath).then(function(repository) {
      test.repository = repository;
      done();
    });
  });

  it("gets no statuses on clean working directory", function() {
    var statuses = [];
    var statusCallback = function(path, status) {
      statuses.push({path: path, status: status});
    };
    return Status.foreach(this.repository, statusCallback).then(function() {
      assert.equal(statuses.length, 0);
    });
  });

  it("gets a status on changing file directory", function() {
    var fileName = "README.md";
    var fileContent = "Cha-cha-cha-chaaaaaangessssss";
    var repo = this.repository;
    var oldContent;
    var filePath = path.join(repo.workdir(), fileName);

    return fse.readFile(filePath).then(function(content) {
      oldContent = content;
      return fse.writeFile(filePath, fileContent)
        .then(function() {
          var statuses = [];
          var statusCallback = function(path, status) {
            statuses.push({path: path, status: status});
          };
          return Status.foreach(repo, statusCallback).then(function() {
            assert.equal(statuses.length, 1);
            assert.equal(statuses[0].path, fileName);
            assert.equal(statuses[0].status, 256);
            return fse.writeFile(filePath, oldContent);
          });
        });
    });
  });

  it("gets status with options", function() {
    var fileName = "my-new-file-that-shouldnt-exist";
    var fileContent = "new file";
    var repo = this.repository;
    var filePath = path.join(repo.workdir(), fileName);

    return fse.writeFile(filePath, fileContent)
      .then(function() {
        var statuses = [];
        var statusCallback = function(path, status) {
          statuses.push({path: path, status: status});
        };

        var opts = {
          flags: Status.OPT.INCLUDE_UNTRACKED +
                 Status.OPT.RECURSE_UNTRACKED_DIRS
        };

        return Status.foreachExt(repo, opts, statusCallback).then(function() {
          assert.equal(statuses.length, 1);
          assert.equal(statuses[0].path, fileName);
          assert.equal(statuses[0].status, 128);
          return fse.unlink(filePath);
        });
      });
  });
});
