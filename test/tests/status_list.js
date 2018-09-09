var assert = require("assert");
var path = require("path");
var fse = require("fs-extra");
var local = path.join.bind(path, __dirname);
var exec = require("../../utils/execPromise");

describe("StatusList", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Status = NodeGit.Status;
  var StatusList = NodeGit.StatusList;

  var reposPath = local("../repos/workdir");

  before(function() {
    var test = this;
    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;
      });
  });

  it("gets status with deltas", function() {
    var fileName = "my-new-file-that-shouldnt-exist.file";
    var fileContent = "new file from status tests";
    var repo = this.repository;
    var filePath = path.join(repo.workdir(), fileName);
    return exec("git clean -xdf", {cwd: reposPath})
      .then(function() {
        return fse.writeFile(filePath, fileContent);
      })
      .then(function() {
        var opts = {
          flags: Status.OPT.INCLUDE_UNTRACKED +
                 Status.OPT.RECURSE_UNTRACKED_DIRS
        };

        return StatusList.create(repo, opts);
      })
      .then(function(list) {
        assert.equal(list.entrycount(), 1);

        for (var i = 0; i < list.entrycount(); i++) {
          var entry = Status.byIndex(list, i);
          assert.equal(entry.indexToWorkdir().newFile().path(), fileName);
        }
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
