var assert = require("assert");
var path = require("path");
var promisify = require("promisify-node");
var Promise = require("nodegit-promise");
var fse = promisify(require("fs-extra"));
var local = path.join.bind(path, __dirname);
var exec = promisify(function(command, opts, callback) {
  return require("child_process").exec(command, opts, callback);
});

describe.only("StatusList", function() {
  var Status = require(local("../../lib/status"));
  var StatusList = require(local("../../lib/status_list"));
  var Repository = require(local("../../lib/repository"));

  var reposPath = local("../repos/workdir/.git");

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
    return exec("git clean -xdf", {cwd: local("../repos/workdir")})
      .then(function() {
        return fse.writeFile(filePath, fileContent);
      })
      .then(function() {
        var opts = {
          flags: Status.OPT.INCLUDE_UNTRACKED +
                 Status.OPT.RECURSE_UNTRACKED_DIRS
        };

        var list = StatusList.create(repo, opts);
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
