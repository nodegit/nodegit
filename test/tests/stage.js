var assert = require("assert");
var path = require("path");
var Promise = require("nodegit-promise");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));

describe("Stage", function() {
  var RepoUtils = require("../utils/repository_setup");
  var NodeGit = require("../../");
	var test;

  beforeEach(function() {
    test = this;
		var repoDir = "../repos/stagingRepo/";
    var repoPath = path.resolve(__dirname, repoDir);

    return RepoUtils.createRepository(repoPath)
      .then(function(repo) {
        test.repository = repo;
        return repo.openIndex();
      })
      .then(function(index) {
        test.index = index;
      });
  });

  after(function() {
		return fse.remove(test.repository.workdir());
  });

function stagingTest(staging) {
    var fileContent = "One line of text\n" +
                      "Two lines of text\n"+
                      "Three lines of text\n"+
                      "Four lines of text\n"+
                      "Five lines of text\n"+
                      "Six lines of text\n"+
                      "Seven lines of text\n"+
                      "Eight lines of text\n"+
                      "Nine lines of text\n"+
                      "Ten lines of text\n"+
                      "Eleven lines of text\n"+
                      "Twelve lines of text\n"+
                      "Thirteen lines of text\n"+
                      "Fourteen lines of text\n"+
                      "Fifteen lines of text\n"+
                      "Sixteen lines of text\n"+
                      "Seventeen lines of text\n"+
                      "Eighteen lines of text\n"+
                      "Nineteen lines of text\n"+
                      "Twenty lines of text\n";
    var fileName = "stagedLinesTest.txt";
    var index;
    var stagedFile;
    var workingDirFile;
    var getDiffFunction;
    if (staging) {
      stagedFile = fileContent.replace("Fifteen", "Changed fifteen");
      workingDirFile = stagedFile.replace("Three", "Changed three")
                            .replace("Seventeen", "Changed seventeen");
      getDiffFunction = function() {
        return NodeGit.Diff.indexToWorkdir(test.repository, index, {
            flags:
              NodeGit.Diff.OPTION.SHOW_UNTRACKED_CONTENT |
              NodeGit.Diff.OPTION.RECURSE_UNTRACKED_DIRS
          });
      };
    }
    else {
      stagedFile = fileContent.replace("Three", "Changed three")
                            .replace("Seventeen", "Changed seventeen");
      workingDirFile = stagedFile.replace("Fifteen", "Changed fifteen");

      getDiffFunction = function() {
        return RepoUtils.addFileToIndex(test.repository, fileName)
          .then(function() {
            return test.repository.getBranchCommit("master");
          })
          .then(function(masterCommit) {
            return masterCommit.getTree();
          })
          .then(function(masterTree) {
            return NodeGit.Diff.treeToIndex(
							test.repository, masterTree, index, {
              flags:
                NodeGit.Diff.OPTION.SHOW_UNTRACKED_CONTENT |
                NodeGit.Diff.OPTION.RECURSE_UNTRACKED_DIRS
            });
          });
      };
    }

    return RepoUtils.commitFileToRepo(test.repository, fileName, fileContent)
    .then(function() {
      return fse.writeFile(path.join(test.repository.workdir(), fileName),
                            workingDirFile);
    })
    .then(function() {
      return test.repository.openIndex();
    })
    .then(function(repoIndex) {
      index = repoIndex;
      return getDiffFunction();
    })
    .then(function(fileDiff) {
      return fileDiff.patches();
    })
    .then(function(patches) {
      var pathPatch = patches.filter(function(patch) {
        return patch.newFile().path() === fileName;
      });
      return pathPatch[0].hunks();
    })
    .then(function(pathHunks) {
      var linePromises = [];

      pathHunks.forEach(function(pathHunk) {
        linePromises.push(pathHunk.lines());
      });

      return Promise.all(linePromises);
    })
    .then(function(lines) {
      var linesToStage = [];
      lines.forEach(function(hunkLines) {
        hunkLines.forEach(function(line) {
          if (line.content().toLowerCase().indexOf("fifteen") >= 0){
            linesToStage.push(line);
          }
        });
      });
      return test.repository.stageLines(fileName, linesToStage, !staging);
    })
    .then(function(stageResult) {
      assert.equal(stageResult, 0);
      var pathOid = index.getByPath(fileName).id;
      return test.repository.getBlob(pathOid);
    })
    .then(function(resultFileContents) {
      assert.equal(resultFileContents, stagedFile);
    });
  }

  it("can stage selected lines", function() {
    return stagingTest(true);
  });
  it("can unstage selected lines", function() {
    return stagingTest(false);
  });
});
