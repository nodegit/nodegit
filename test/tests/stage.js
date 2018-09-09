var assert = require("assert");
var path = require("path");
var fse = require("fs-extra");

var exec = require("../../utils/execPromise");

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
      });
  });

  after(function() {
		return fse.remove(test.repository.workdir());
  });

  function stagingTest(isUnstaged, newFileContent, discarding) {
    var fileContent = newFileContent ||
                      "One line of text\n" +
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
    var expectedContent;
    var workingDirFile;
    var getDiffFunction;

    if (!isUnstaged || discarding) {
      expectedContent = fileContent.replace("Three", "Changed three")
                            .replace("Seventeen", "Changed seventeen");
      workingDirFile = expectedContent.replace("Fifteen", "Changed fifteen");
    }
    else {
      expectedContent = fileContent.replace("Fifteen", "Changed fifteen");
      workingDirFile = expectedContent.replace("Three", "Changed three")
                            .replace("Seventeen", "Changed seventeen");
    }

    if (isUnstaged) {
      getDiffFunction = function() {
        return test.repository.refreshIndex()
          .then(function(index) {
            return NodeGit.Diff.indexToWorkdir(
              test.repository,
              index,
              {
                flags:
                  NodeGit.Diff.OPTION.SHOW_UNTRACKED_CONTENT |
                  NodeGit.Diff.OPTION.RECURSE_UNTRACKED_DIRS
              }
            );
          });
      };
    }
    else {
      getDiffFunction = function() {
        return RepoUtils.addFileToIndex(test.repository, fileName)
          .then(function() {
            return test.repository.getBranchCommit("master");
          })
          .then(function(masterCommit) {
            var treePromise = masterCommit.getTree();
            var indexPromise = test.repository.refreshIndex();

            return Promise.all([treePromise, indexPromise]);
          })
          .then(function(treeAndIndex) {
            var masterTree = treeAndIndex[0];
            var index = treeAndIndex[1];
            return NodeGit.Diff.treeToIndex(
              test.repository,
              masterTree,
              index,
              {
                flags:
                  NodeGit.Diff.OPTION.SHOW_UNTRACKED_CONTENT |
                  NodeGit.Diff.OPTION.RECURSE_UNTRACKED_DIRS
              }
            );
          });
      };
    }

    return RepoUtils.commitFileToRepo(test.repository, fileName, fileContent)
    .then(function() {
      return fse.writeFile(path.join(test.repository.workdir(), fileName),
                            workingDirFile);
    })
    .then(function() {
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

      if (discarding) {
        return test.repository.discardLines(fileName, linesToStage);
      }

      return test.repository.stageLines(fileName, linesToStage, !isUnstaged);
    })
    .then(function() {
      if (discarding) {
        return fse.readFile(
          path.join(test.repository.workdir(), fileName), "utf8"
        );
      }

      return test.repository.refreshIndex()
        .then(function(reloadedIndex) {
          var pathOid = reloadedIndex.getByPath(fileName).id;
          return test.repository.getBlob(pathOid);
        });
    })
    .then(function(resultFileContents) {
      assert.equal(resultFileContents.toString(), expectedContent);
    });
  }

  it("can stage selected lines", function() {
    return stagingTest(true);
  });
  it("can unstage selected lines", function() {
    return stagingTest(false);
  });

  //This is used to test cases where there are no newline at EOF
  var newlineEofTestFileContent =
                  "One line of text\n" +
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
                  "Fifteen lines of text";
  it("can stage last line with no newline at EOF", function() {
    return stagingTest(true, newlineEofTestFileContent);
  });
  it("can unstage last line with no newline at EOF", function() {
    return stagingTest(false, newlineEofTestFileContent);
  });
  it("can stage second to last line with no newline at EOF", function() {
    var newlineEofTestFileContent2 = newlineEofTestFileContent +
     "\nSixteen lines of text\nSeventeen lines of text\nEighteen lines of text";
    return stagingTest(true, newlineEofTestFileContent2);
  });
  it("can unstage second to last line with no newline at EOF", function() {
    var newlineEofTestFileContent2 = newlineEofTestFileContent +
     "\nSixteen lines of text\nSeventeen lines of text\nEighteen lines of text";
    return stagingTest(false, newlineEofTestFileContent2);
  });

  //This is used to test case where the last hunk is staged.
  var lastHunkStagedFileContent =
                "Thirteen lines of text\n"+
                "Fourteen lines of text\n"+
                "Fifteen lines of text\n"+
                "Sixteen lines of text\n"+
                "Shforteenteen lines of text\n";

  it("staging last hunk stages whole file if no filemode changes", function() {
    return stagingTest(true, lastHunkStagedFileContent)
      .then(function() {
        return test.repository.refreshIndex();
      })
      .then(function(index) {
        return NodeGit.Diff.indexToWorkdir(test.repository, index, {
            flags:
              NodeGit.Diff.OPTION.SHOW_UNTRACKED_CONTENT |
              NodeGit.Diff.OPTION.RECURSE_UNTRACKED_DIRS
          });
      })
      .then(function(diff) {
        assert.equal(Object.keys(diff).length, 0);  //Empty diff
        return diff.patches();
      })
      .then(function(patches) {
        //patches will have at least one item if there is something unstaged
        assert.equal(patches.length, 0);
      });
  });

  function compareFilemodes(vsWorkdir, index, fileModeDifference) {
    //Takes diff of head commit vs Workdir (if vsWorkdir is set) or vs Index
    //(if vsWorkdir is unset). Note: there's only one file in the filemode
    //staging tests for which this helper fn was written.
    //index - index to use (vsWorkdir is unset)
    //fileModeDifference - expected (newfilemode) - (oldfilemode)
    return test.repository.getHeadCommit()
      .then(function(commit) {
        return commit.getTree();
      })
      .then(function(tree) {
        if (vsWorkdir) {
          return NodeGit.Diff.treeToWorkdir(test.repository, tree);
        } else {
          return NodeGit.Diff.treeToIndex(test.repository, tree, index);
        }
      })
      .then(function(diff) {
        return diff.getDelta(0);
      })
      .then(function(delta) {
        if (fileModeDifference === 0) {
          if (!delta) {
            return true;
          } else {
            throw ("File change when no file change expected.");
          }
        } else {
          assert(delta.newFile().mode() - delta.oldFile().mode() ===
            fileModeDifference);
        }
        return true;
      });
  }

  function createAndCommitFiles(repo, filePaths, fileContent, afterWriteFn) {
    filePaths = filePaths instanceof Array ? filePaths : [filePaths];
    var filePromises = filePaths.map(function(fileName) {
      return RepoUtils.commitFileToRepo(repo, fileName, fileContent)
      .then(function() {
        //First, create a file, have the same file in both the repo and workdir.
        return fse.writeFile(path.join(repo.workdir(), fileName), fileContent);
      })
      .then(function() {
        return afterWriteFn(repo, fileName);
      });
    });

    return Promise.all(filePromises);
  }

  if (process.platform == "linux" || process.platform == "darwin") {
    it("can stage filemode changes for one file", function() {
      var fileContent = "Blek";
      var fileName = "stageFilemodeTest.txt";
      var index;

      function afterWriteFn(repo, fileName) {
        return fse.chmod(path.join(repo.workdir(), fileName),
          0755 /* new filemode */);
      }

      return createAndCommitFiles(
        test.repository, fileName, fileContent, afterWriteFn
      )
      //Then, diff between head commit and workdir should have filemode change
      .then(function() {
        return compareFilemodes(true, null, 0111 /* expect +x */)
          .then(function() {
            return test.repository.stageFilemode(fileName, true);
          });
      })
      //Now lets do a commit...
      .then(function() {
        return test.repository.refreshIndex();
      })
      .then(function(_index) {
        index = _index;
        return index.writeTree();
      })
      .then(function (oid) {
        return test.repository.getHeadCommit()
          .then(function(parent) {
            var signature = NodeGit.Signature.create("Foo bar",
            "foo@bar.com", 123456789, 60);
            return test.repository.createCommit("HEAD", signature, signature,
            "initial commit", oid, [parent]);
          });
        //... alright, we did a commit.
      })
      // Now if we compare head commit to the workdir,
      // there shouldn't be a filemode change
      .then(function() {
        return compareFilemodes(true, null, 0);
      });
    });

    it("can unstage filemode changes", function() {
      var fileContent = "Blek";
      var fileName = "stageFilemodeTest2.txt";
      var index;

      function afterWriteFn(repo, fileName) {
        return fse.chmod(path.join(repo.workdir(), fileName),
          0755 /* new filemode */);
      }

      return createAndCommitFiles(
        test.repository,
        fileName,
        fileContent,
        afterWriteFn
      )
      //Then, diff between head commit and workdir should have filemode change
      .then(function() {
        return compareFilemodes(true, null, 0111 /* expect +x */);
      })
      .then(function() {
        return test.repository.refreshIndex();
      })
      .then(function(repoIndex) {
        //Now we stage the whole file...
        index = repoIndex;
        return index.addByPath(fileName);
      })
      .then(function() {
        return index.write();
      })
      .then(function() {
        //We expect the Index to have the filemode changes now.
        return compareFilemodes(false, index, 0111 /* expect +x */)
          .then(function() {
            //...then we attempt to unstage filemode
            return test.repository.stageFilemode(fileName, false /* unstage */);
          });
      })
      .then(function() {
        return test.repository.refreshIndex();
      })
      //We expect the Index to have no filemode changes, since we unstaged.
      .then(function(freshIndex) {
        return compareFilemodes(false, freshIndex, 0 /* expect +x */);
      })
      //We also expect the workdir to now have the filemode change.
      .then(function() {
        return compareFilemodes(true, null, 0111 /* expect +x */);
      });
    });
  } else if (process.platform == "win32") {
    it("can stage/unstage filemode changes for one file", function() {
      var fileContent = "Blek";
      var fileName = "stageFilemodeTest.txt";
      var index;

      function afterWriteFn(repo, fileName) {
        //change the permission on index
        return exec("git update-index --chmod=+x " + fileName,
          {cwd: repo.workdir()})
        .then(function() {
          //Commit the change with execute bit set
          return exec("git commit -m 'test'",
            {cwd: repo.workdir()});
        })
        .then(function() {
          //Then, change the permission on index
          return exec("git update-index --chmod=-x " + fileName,
            {cwd: repo.workdir()});
        });
      }

      return createAndCommitFiles(
        test.repository, fileName, fileContent, afterWriteFn
      )
      .then(function() {
        return test.repository.refreshIndex();
      })
      .then(function(repoIndex) {
        index = repoIndex;
        //Head commit vs index
        //We expect the Index to have +x
        return compareFilemodes(false, index, -0111 /* expect +x */);
      })
      .then(function() {
        //...then we attempt to unstage filemode
        return test.repository.stageFilemode(fileName, false /* unstage */);
      })
      .then(function() {
        return test.repository.refreshIndex();
      })
      .then(function(freshIndex) {
        return compareFilemodes(false, freshIndex, 0 /* expect nochange */);
      });
    });
  }

  it("can stage/unstage filemode changes for multiple files", function() {
    var fileContent = "Blek";
    var fileName = ["stageFilemodeTest.txt", "stageFilemodeTest2.txt"];
    var index;

    var repoWorkDir = test.repository.workdir();
    var signature = NodeGit.Signature.create("Foo bar",
      "foo@bar.com", 123456789, 60);

    return Promise.all(fileName.map(function(file) {
      return fse.writeFile(path.join(repoWorkDir, file), fileContent);
    }))
    .then(function() {
      // Initial commit
      return test.repository.refreshIndex();
    })
    .then(function(index) {
      return fileName
        .reduce(function(lastPromise, file) {
          return lastPromise
            .then(function() {
              return index.addByPath(file);
            });
        }, Promise.resolve())
        .then(function() {
          return index.write();
        })
        .then(function() {
          return index.writeTree();
        });
    })
    .then(function(oid) {
      return test.repository.createCommit("HEAD", signature, signature,
        "initial commit", oid, []);
    })
    .then(function(commitOid) {
      return test.repository.getCommit(commitOid);
    })
    .then(function() {
      //change the permission on index
      return exec("git update-index --chmod=+x " + fileName[0],
        {cwd: test.repository.workdir()});
    })
    .then(function() {
      //change the permission on index
      return exec("git update-index --chmod=+x " + fileName[1],
        {cwd: test.repository.workdir()});
    })
    .then(function() {
        //Commit the change with execute bit set
        return exec("git commit -m 'test'",
          {cwd: test.repository.workdir()});
    })
    .then(function() {
      //Then, change the permission on index back to -x
      return exec("git update-index --chmod=-x " + fileName[0],
        {cwd: test.repository.workdir()});
    })
    .then(function() {
      //Then, change the permission on index back to -x
      return exec("git update-index --chmod=-x " + fileName[1],
        {cwd: test.repository.workdir()});
    })
    .then(function() {
      return test.repository.refreshIndex();
    })
    .then(function(repoIndex) {
      index = repoIndex;
      //Head commit vs index
      //We expect the Index to have +x
      return compareFilemodes(false, index, -0111 /* expect +x */);
    })
    .then(function() {
      //...then we attempt to unstage filemode
      return test.repository.stageFilemode(fileName, false /* unstage */);
    })
    .then(function() {
      return test.repository.refreshIndex();
    })
    .then(function(freshIndex) {
      return compareFilemodes(false, freshIndex, 0 /* expect nochange */);
    });
  });

  it("can discard selected lines", function() {
    return stagingTest(true, null, true);
  });
});
