var assert = require("assert");
var path = require("path");
var promisify = require("promisify-node");
var fse = promisify(require("fs-extra"));
fse.ensureDir = promisify(fse.ensureDir);

describe("Merge", function() {
  var nodegit = require("../../");

  var repoDir = path.resolve("test/repos/merge");
  var ourBranchName = "ours";
  var theirBranchName = "theirs";

  beforeEach(function() {
    var test = this;
    return fse.remove(path.resolve(__dirname, repoDir))
      .then(function() {
        return fse.ensureDir(path.resolve(__dirname, repoDir));
      })
      .then(function() {
        return nodegit.Repository.init(path.resolve(__dirname, repoDir), 0);
      })
      .then(function(repo) {
        test.repository = repo;
      });
  });

  it("can cleanly merge 2 files", function() {
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = nodegit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = nodegit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var theirCommit;
    var ourBranch;
    var theirBranch;

    return fse.writeFile(path.join(repository.workdir(), ourFileName),
        ourFileContent)
      // Load up the repository index and make our initial commit to HEAD
      .then(function() {
        return repository.openIndex()
          .then(function(index) {
            index.read(1);
            index.addByPath(ourFileName);
            index.write();

            return index.writeTree();
          });
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "11ead82b1135b8e240fb5d61e703312fb9cc3d6a");

        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "we made a commit", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "91a183f87842ebb7a9b08dad8bc2473985796844");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        }).then(function() {
          return repository.createBranch(ourBranchName, commitOid)
            .then(function(branch) {
              ourBranch = branch;
              return repository.createBranch(theirBranchName, commitOid);
            });
        });
      })
      .then(function(branch) {
        theirBranch = branch;
        return fse.writeFile(path.join(repository.workdir(), theirFileName),
          theirFileContent);
      })
      .then(function() {
        return repository.openIndex()
          .then(function(index) {
            index.read(1);
            index.addByPath(theirFileName);
            index.write();

            return index.writeTree();
          });
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "76631cb5a290dafe2959152626bb90f2a6d8ec94");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "0e9231d489b3f4303635fc4b0397830da095e7e7");

        return repository.getCommit(commitOid).then(function(commit) {
          theirCommit = commit;
        });
      })
      .then(function() {
        return nodegit.Merge.commits(repository, ourCommit, theirCommit);
      })
      .then(function(index) {
        assert(!index.hasConflicts());
        index.write();
        return index.writeTreeTo(repository);
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "76631cb5a290dafe2959152626bb90f2a6d8ec94");

        return repository.createCommit(ourBranch.name(), ourSignature,
          ourSignature, "we merged their commit", oid,
          [ourCommit, theirCommit]);
      })
      .then(function(commitId) {
        assert.equal(commitId.toString(),
          "eedee554af34dd4001d8abc799cb55bb7e56a58b");
      });
  });

  it("can fast-forward using the convenience method", function() {
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = nodegit.Signature.create
    ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = nodegit.Signature.create
    ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var ourCommit;
    var theirCommit;
    var ourBranch;
    var theirBranch;

    return fse.writeFile(
      path.join(repository.workdir(), ourFileName),
      ourFileContent)
    // Load up the repository index and make our initial commit to HEAD
    .then(function() {
      return repository.openIndex()
      .then(function(index) {
        index.read(1);
        index.addByPath(ourFileName);
        index.write();

        return index.writeTree();
      });
    })
    .then(function(oid) {
      assert.equal(oid.toString(),
      "11ead82b1135b8e240fb5d61e703312fb9cc3d6a");

      return repository.createCommit("HEAD", ourSignature,
      ourSignature, "we made a commit", oid, []);
    })
    .then(function(commitOid) {
      assert.equal(commitOid.toString(),
      "91a183f87842ebb7a9b08dad8bc2473985796844");

      return repository.getCommit(commitOid).then(function(commit) {
        ourCommit = commit;
      }).then(function() {
        return repository.createBranch(ourBranchName, commitOid)
        .then(function(branch) {
          ourBranch = branch;
          return repository.createBranch(theirBranchName, commitOid);
        });
      });
    })
    .then(function(branch) {
      theirBranch = branch;
      return fse.writeFile(path.join(repository.workdir(), theirFileName),
      theirFileContent);
    })
    .then(function() {
      return repository.openIndex()
      .then(function(index) {
        index.read(1);
        index.addByPath(theirFileName);
        index.write();

        return index.writeTree();
      });
    })
    .then(function(oid) {
      assert.equal(oid.toString(),
      "76631cb5a290dafe2959152626bb90f2a6d8ec94");

      return repository.createCommit(theirBranch.name(), theirSignature,
      theirSignature, "they made a commit", oid, [ourCommit]);
    })
    .then(function(commitOid) {
      assert.equal(commitOid.toString(),
      "0e9231d489b3f4303635fc4b0397830da095e7e7");

      return repository.getCommit(commitOid).then(function(commit) {
        theirCommit = commit;
      });
    })
    .then(function() {
      return repository.mergeBranches(ourBranchName, theirBranchName);
    })
    .then(function(oid) {
      assert.equal(oid.toString(),
      "0e9231d489b3f4303635fc4b0397830da095e7e7");

      return repository.getBranchCommit(ourBranchName).then(function(branchCommit) {
        assert.equal(oid.toString(), branchCommit.toString());
      });
    });
  });

  it.only("can merge cleanly using the convenience method", function() {
    var initialFileName = "initialFile.txt";
    var ourFileName = "ourNewFile.txt";
    var theirFileName = "theirNewFile.txt";

    var initialFileContent = "I'd like to drive somewhere";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
    var theirFileContent = "I'm skeptical about Toll Roads";

    var ourSignature = nodegit.Signature.create
    ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = nodegit.Signature.create
    ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var repository = this.repository;
    var initialCommit;
    var ourCommit;
    var theirCommit;
    var ourBranch;
    var theirBranch;

    return fse.writeFile(
      path.join(repository.workdir(), initialFileName),
      initialFileContent)
    // Load up the repository index and make our initial commit to HEAD
    .then(function() {
      return repository.openIndex()
      .then(function(index) {
        index.read(1);
        index.addByPath(initialFileName);
        index.write();

        return index.writeTree();
      });
    })
    .then(function(oid) {
      assert.equal(oid.toString(),
      "21a553813e2f670815b649eef51eeadb253a5d0c");

      return repository.createCommit("HEAD", ourSignature,
      ourSignature, "initial commit", oid, []);
    })
    .then(function(commitOid) {
      assert.equal(commitOid.toString(),
      "af66a9c799a10a23319ee4318c8bb2021521f539");

      return repository.getCommit(commitOid).then(function(commit) {
        initialCommit = commit;
      }).then(function() {
        return repository.createBranch(ourBranchName, commitOid)
        .then(function(branch) {
          ourBranch = branch;
          return repository.createBranch(theirBranchName, commitOid);
        });
      });
    })
    .then(function(branch) {
      theirBranch = branch;
    })
    .then(function() {
      return fse.writeFile(path.join(repository.workdir(), ourFileName),
      ourFileContent);
    })
    .then(function() {
      return repository.openIndex()
      .then(function(index) {
        index.read(1);
        index.addByPath(ourFileName);
        index.write();

        return index.writeTree();
      });
    })
    .then(function(oid) {
      assert.equal(oid.toString(),
      "af60aa06b3537f75b427f6268a130c842c84a137");

      return repository.createCommit(ourBranch.name(), ourSignature,
        ourSignature, "we made a commit", oid, [initialCommit]);
    })
    .then(function(commitOid) {
      assert.equal(commitOid.toString(),
        "7ce31c05427659986d50abfb90c8f7db88ef4fa1");

      return repository.getCommit(commitOid).then(function(commit) {
        ourCommit = commit;
      });
    })
    .then(function() {
      return fse.writeFile(path.join(repository.workdir(), theirFileName),
      theirFileContent);
    })
    .then(function() {
      return repository.openIndex()
      .then(function(index) {
        index.read(1);
        index.addByPath(theirFileName);
        index.write();

        return index.writeTree();
      });
    })
    .then(function(oid) {
      assert.equal(oid.toString(),
      "f007361737a2ca00a0e80fc2daf55064463173b4");

      return repository.createCommit(theirBranch.name(), theirSignature,
      theirSignature, "they made a commit", oid, [initialCommit]);
    })
    .then(function(commitOid) {
      assert.equal(commitOid.toString(),
      "b588f0eef1809226f8f7db542940749da15ae1de");

      return repository.getCommit(commitOid).then(function(commit) {
        theirCommit = commit;
      });
    })
    .then(function() {
      return repository.mergeBranches(ourBranchName, theirBranchName);
    })
    .then(function(commitId) {
      assert.equal(commitId.toString(),
      "0e9231d489b3f4303635fc4b0397830da095e7e7");
    });
  });

  it("can merge 2 branchs with conflicts on a single file", function () {
    var baseFileContent = "All Bobs are created equal. ish.\n";
    var ourFileContent = "Big Bobs are best, IMHO.\n";
    var theirFileContent = "Nobody expects the small Bobquisition!\n";
    var finalFileContent =
      "Big Bobs are beautiful, and the small are unexpected!\n";

    var baseSignature = nodegit.Signature.create
          ("Peaceful Bob", "justchill@bob.net", 123456789, 60);
    var ourSignature = nodegit.Signature.create
          ("Big Bob", "impressive@bob.net", 123456789, 60);
    var theirSignature = nodegit.Signature.create
          ("Small Bob", "underestimated@bob.net", 123456789, 60);

    var repository = this.repository;
    var baseCommit;
    var baseCommitOid;
    var ourCommit;
    var theirCommit;
    var ourBranch;
    var theirBranch;
    var fileName = "newFile.txt";

    return fse.writeFile(path.join(repository.workdir(), fileName),
      baseFileContent)
      .then(function() {
        return repository.openIndex()
          .then(function(index) {
            index.read(1);
            index.addByPath(fileName);
            index.write();

            return index.writeTree();
          });
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "ea2f6521fb8097a881f43796946ac1603e1f4d75");

        return repository.createCommit("HEAD", baseSignature,
        baseSignature, "bobs are all ok", oid, []);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "a9b202f7612273fb3a68f718304298704eaeb735");
        baseCommitOid = commitOid;

        return repository.getCommit(commitOid).then(function(commit) {
          baseCommit = commit;
        });
      })
      .then(function() {
        return repository.createBranch(ourBranchName, baseCommitOid)
          .then(function(branch) {
            ourBranch = branch;
          });
      })
      .then(function() {
        return repository.createBranch(theirBranchName, baseCommitOid)
          .then(function(branch) {
            theirBranch = branch;
          });
      })
      .then(function() {
        return fse.writeFile(path.join(repository.workdir(), fileName),
          ourFileContent);
      })
      .then(function() {
        return repository.openIndex().then(function(index) {
          index.read(1);
          index.addByPath(fileName);
          index.write();

          return index.writeTree();
        });
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "c39b1e38b09085856cec7e7ff33e90f5a537d8a5");

        return repository.createCommit(ourBranch.name(), ourSignature,
          ourSignature, "lol big bobs :yesway:", oid, [baseCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "935a89c09ad757a9dde2c0257f6f1e379f71816f");

        return repository.getCommit(commitOid).then(function(commit) {
          ourCommit = commit;
        });
      })
      .then(function() {
        return fse.writeFile(path.join(repository.workdir(), fileName),
          theirFileContent);
      })
      .then(function() {
        return repository.openIndex().then(function(index) {
          index.read(1);
          index.addByPath(fileName);
          index.write();

          return index.writeTree();
        });
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "d1a894a9a4a8c820eb66c82cdd7e6b76c8f713cb");

        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "lol big bobs :poop:", oid, [baseCommit]);
      })
      .then(function(commitOid) {
        assert.equal(commitOid.toString(),
          "bebb9ec2e0684c7cb7c1e1601c7d5a8f52b8b123");

        return repository.getCommit(commitOid).then(function(commit) {
          theirCommit = commit;
        });
      })
      .then(function() {
        return nodegit.Reference.lookup(repository, "HEAD")
          .then(function(head) {
            return head.symbolicSetTarget(ourBranch.name(), ourSignature, "");
          });
      })
      .then(function() {
        return nodegit.Merge.commits(repository, ourCommit, theirCommit, null);
      })
      .then(function(index) {
        assert(index.hasConflicts());
        fse.writeFileSync(path.join(repository.workdir(), fileName),
          finalFileContent);
      })
      .then(function() {
        return repository.openIndex().then(function(index) {
          index.read(1);
          index.addByPath(fileName);
          index.write();

          return index.writeTree();
        });
      })
      .then(function(oid) {
        assert.equal(oid.toString(),
          "b1cd49a27cd33b99ab6dad2fb82b3174812a8b47");

        return repository.createCommit(ourBranch.name(), baseSignature,
          baseSignature, "Stop this bob sized fued", oid,
          [ourCommit, theirCommit]);
      })
      .then(function(commitId) {
        assert.equal(commitId.toString(),
          "49014ccabf5125f9b69316acde36f891dfdb8b5c");
      });
  });
});
