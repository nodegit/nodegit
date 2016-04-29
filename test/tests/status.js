var assert = require("assert");
var path = require("path");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));
var local = path.join.bind(path, __dirname);
var exec = require("../../utils/execPromise");

describe("Status", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Status = NodeGit.Status;

  var reposPath = local("../repos/workdir");

  before(function() {
    var test = this;
    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      });
  });

  it("gets no statuses on clean working directory", function() {
    var statuses = [];
    var statusCallback = function(path, status) {
      statuses.push({path: path, status: status});
    };

    return Status.foreach(this.repository, statusCallback)
      .then(function() {
        assert.equal(statuses.length, 0);
      });
  });

  it("gets a status on changing file directory", function() {
    var fileName = "README.md";
    var fileContent = "Cha-cha-cha-chaaaaaangessssss";
    var repo = this.repository;
    var filePath = path.join(repo.workdir(), fileName);
    var oldContent;
    var statuses = [];

    return fse.readFile(filePath)
      .then(function(content) {
        oldContent = content;
        return fse.writeFile(filePath, fileContent);
      })
      .then(function() {
        var statusCallback = function(path, status) {
          statuses.push({path: path, status: status});
        };
        return Status.foreach(repo, statusCallback);
      })
      .then(function() {
        assert.equal(statuses.length, 1);
        assert.equal(statuses[0].path, fileName);
        assert.equal(statuses[0].status, 256);
      })
      .then(function () {
        return fse.writeFile(filePath, oldContent);
      })
      .catch(function(e) {
        return fse.writeFile(filePath, oldContent)
          .then(function() {
            return Promise.reject(e);
          });
      });
  });

  it("gets status with options", function() {
    var fileName = "my-new-file-that-shouldnt-exist.file";
    var fileContent = "new file from status tests";
    var repo = this.repository;
    var filePath = path.join(repo.workdir(), fileName);
    return exec("git clean -xdf", {cwd: reposPath})
      .then(function() {
        return fse.writeFile(filePath, fileContent);
      })
      .then(function() {
        var statuses = [];
        var statusCallback = function(path, status) {
          statuses.push({path: path, status: status});
        };

        var opts = {
          flags: Status.OPT.INCLUDE_UNTRACKED +
                 Status.OPT.RECURSE_UNTRACKED_DIRS
        };

        return Status.foreachExt(repo, opts, statusCallback)
          .then(function() {
            assert.equal(statuses.length, 1);
            assert.equal(statuses[0].path, fileName);
            assert.equal(statuses[0].status, 128);
          });
      })
      .then(function() {
        return fse.remove(filePath);
      })
      .catch(function(e) {
        return fse.remove(filePath)
          .then(function() {
            return Promise.reject(e);
          });

      });
  });
});
