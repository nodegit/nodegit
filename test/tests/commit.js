var assert = require("assert");
var path = require("path");
var fse = require("fs-extra");

var garbageCollect = require("../utils/garbage_collect.js");
var leakTest = require("../utils/leak_test");

var local = path.join.bind(path, __dirname);

var exec = require("../../utils/execPromise");

describe("Commit", function() {
  var NodeGit = require("../../");
  var Repository = NodeGit.Repository;
  var Commit = NodeGit.Commit;
  var Diff = NodeGit.Diff;
  var Oid = NodeGit.Oid;

  var reposPath = local("../repos/workdir");
  var oid = "fce88902e66c72b5b93e75bdb5ae717038b221f6";

  function reinitialize(test) {
    return Repository.open(reposPath)
      .then(function(repository) {
        test.repository = repository;

        return repository.getCommit(oid);
      })
      .then(function(commit) {
        test.commit = commit;
      });
  }

  function commitFile(repo, fileName, fileContent, commitMessage) {
    var index;
    var treeOid;
    var parent;

    return fse.writeFile(path.join(repo.workdir(), fileName), fileContent)
    .then(function() {
      return repo.refreshIndex();
    })
    .then(function(indexResult) {
      index = indexResult;
    })
    .then(function() {
      return index.addByPath(fileName);
    })
    .then(function() {
      return index.write();
    })
    .then(function() {
      return index.writeTree();
    })
    .then(function(oidResult) {
      treeOid = oidResult;
      return NodeGit.Reference.nameToId(repo, "HEAD");
    })
    .then(function(head) {
      return repo.getCommit(head);
    })
    .then(function(parentResult) {
      parent = parentResult;
      return Promise.all([
        NodeGit.Signature.create("Foo Bar", "foo@bar.com", 123456789, 60),
        NodeGit.Signature.create("Foo A Bar", "foo@bar.com", 987654321, 90)
      ]);
    })
    .then(function(signatures) {
      var author = signatures[0];
      var committer = signatures[1];

      return repo.createCommit(
        "HEAD",
        author,
        committer,
        "message",
        treeOid,
        [parent]);
    });
  }

  function undoCommit() {
    return exec("git reset --hard HEAD~1", {cwd: reposPath});
  }

  beforeEach(function() {
    return reinitialize(this);
  });

  it("will fail with an invalid sha", function() {
    return this.repository.getCommit("invalid").then(null, function(err) {
      assert.ok(err instanceof Error);
    });
  });

  it("has a message", function() {
    assert.equal(this.commit.message(), "Update README.md");
  });

  it("has a raw message", function() {
    assert.equal(this.commit.messageRaw(), "Update README.md");
  });

  it("has a message encoding", function() {
    var encoding = this.commit.messageEncoding();
    assert.ok(encoding === "UTF-8" || encoding === undefined);
  });

  it("has a summary", function() {
    assert.equal(this.commit.summary(), "Update README.md");
  });

  it("has a sha", function() {
    assert.equal(this.commit.sha(), oid);
  });

  it("has a time", function() {
    assert.equal(this.commit.timeMs(), 1362012884000);
  });

  it("has a date", function() {
    assert.equal(this.commit.date().getTime(), 1362012884000);
  });

  it("has a time offset", function() {
    assert.equal(this.commit.timeOffset(), 780);
  });

  it("can create a commit", function() {
    var test = this;
    var expectedCommitId = "315e77328ef596f3bc065d8ac6dd2c72c09de8a5";
    var fileName = "newfile.txt";
    var fileContent = "hello world";

    var repo;
    var index;
    var treeOid;
    var parent;

    return NodeGit.Repository.open(reposPath)
    .then(function(repoResult) {
      repo = repoResult;
      return fse.writeFile(path.join(repo.workdir(), fileName), fileContent);
    })
    .then(function() {
      return repo.refreshIndex();
    })
    .then(function(indexResult) {
      index = indexResult;
    })
    .then(function() {
      return index.addByPath(fileName);
    })
    .then(function() {
      return index.write();
    })
    .then(function() {
      return index.writeTree();
    })
    .then(function(oidResult) {
      treeOid = oidResult;
      return NodeGit.Reference.nameToId(repo, "HEAD");
    })
    .then(function(head) {
      return repo.getCommit(head);
    })
    .then(function(parentResult) {
      parent = parentResult;
      return Promise.all([
        NodeGit.Signature.create("Foo Bar", "foo@bar.com", 123456789, 60),
        NodeGit.Signature.create("Foo A Bar", "foo@bar.com", 987654321, 90)
      ]);
    })
    .then(function(signatures) {
      var author = signatures[0];
      var committer = signatures[1];

      return repo.createCommit(
        "HEAD",
        author,
        committer,
        "message",
        treeOid,
        [parent]);
    })
    .then(function(commitId) {
      assert.equal(expectedCommitId, commitId);
      return undoCommit()
      .then(function(){
        return reinitialize(test);
      });
    }, function(reason) {
      return reinitialize(test)
        .then(function() {
          return Promise.reject(reason);
        });
    });
  });

  it("can create a commit as a buffer", function() {
    var test = this;
    var fileName = "newfile.txt";
    var fileContent = "hello world";

    const expectedCommitContent =
      "tree 11c8685af551550e73e5ab89fa554576bd92ef3f\n" +
      "parent 32789a79e71fbc9e04d3eff7425e1771eb595150\n" +
      "author Foo Bar <foo@bar.com> 123456789 +0100\n" +
      "committer Foo A Bar <foo@bar.com> 987654321 +0130\n\n" +
      "message";

    var repo;
    var index;
    var treeOid;
    var parent;

    return NodeGit.Repository.open(reposPath)
    .then(function(repoResult) {
      repo = repoResult;
      return fse.writeFile(path.join(repo.workdir(), fileName), fileContent);
    })
    .then(function() {
      return repo.refreshIndex();
    })
    .then(function(indexResult) {
      index = indexResult;
    })
    .then(function() {
      return index.addByPath(fileName);
    })
    .then(function() {
      return index.write();
    })
    .then(function() {
      return index.writeTree();
    })
    .then(function(oidResult) {
      treeOid = oidResult;
      return NodeGit.Reference.nameToId(repo, "HEAD");
    })
    .then(function(head) {
      return repo.getCommit(head);
    })
    .then(function(parentResult) {
      parent = parentResult;
      return Promise.all([
        NodeGit.Signature.create("Foo Bar", "foo@bar.com", 123456789, 60),
        NodeGit.Signature.create("Foo A Bar", "foo@bar.com", 987654321, 90)
      ]);
    })
    .then(function(signatures) {
      var author = signatures[0];
      var committer = signatures[1];

      return repo.createCommitBuffer(
        author,
        committer,
        "message",
        treeOid,
        [parent]);
    })
    .then(function(commitContent) {
      assert.equal(expectedCommitContent, commitContent);
      return reinitialize(test);
    }, function(reason) {
      return reinitialize(test)
        .then(function() {
          return Promise.reject(reason);
        });
    });
  });

  it("can amend commit", function(){
    var commitToAmendId = "315e77328ef596f3bc065d8ac6dd2c72c09de8a5";
    var expectedAmendedCommitId = "57836e96555243666ea74ea888310cc7c41d4613";
    var fileName = "newfile.txt";
    var fileContent = "hello world";
    var newFileName = "newerfile.txt";
    var newFileContent = "goodbye world";
    var messageEncoding = "US-ASCII";
    var message = "First commit";

    var repo;
    var index;
    var treeOid;
    var parent;
    var author;
    var committer;
    var amendedCommitId;

    return NodeGit.Repository.open(reposPath)
    .then(function(repoResult) {
      repo = repoResult;
      return fse.writeFile(path.join(repo.workdir(), fileName), fileContent);
    })
    .then(function() {
      return repo.refreshIndex();
    })
    .then(function(indexResult) {
      index = indexResult;
    })
    .then(function() {
      return index.addByPath(fileName);
    })
    .then(function() {
      return index.write();
    })
    .then(function() {
      return index.writeTree();
    })
    .then(function(oidResult) {
      treeOid = oidResult;
      return NodeGit.Reference.nameToId(repo, "HEAD");
    })
    .then(function(head) {
      return repo.getCommit(head);
    })
    .then(function(parentResult) {
      parent = parentResult;
      return Promise.all([
        NodeGit.Signature.create("Foo Bar", "foo@bar.com", 123456789, 60),
        NodeGit.Signature.create("Foo A Bar", "foo@bar.com", 987654321, 90)
      ]);
    })
    .then(function(signatures) {
      var author = signatures[0];
      var committer = signatures[1];

      return repo.createCommit(
        "HEAD",
        author,
        committer,
        "message",
        treeOid,
        [parent]);
    })
    .then(function() {
      return fse.writeFile(
        path.join(repo.workdir(), newFileName),
        newFileContent
      );
    })
    .then(function() {
      return repo.refreshIndex();
    })
    .then(function(indexResult) {
      index = indexResult;
    })
    .then(function() {
      return index.addByPath(newFileName);
    })
    .then(function() {
      return index.write();
    })
    .then(function() {
      return index.writeTree();
    })
    .then(function(resultOid){
      treeOid = resultOid;
       return Promise.all([
         repo.getCommit(commitToAmendId),
         NodeGit.Signature.create(
           "New Foo Bar",
           "newfoo@bar.com",
           246802468,
           12
         ),
         NodeGit.Signature.create(
           "New Foo A Bar",
           "newfoo@bar.com",
           4807891730,
           32
         )
       ]);

    })
    .then(function(amendInfo){
      var commit = amendInfo[0];
      author = amendInfo[1];
      committer = amendInfo[2];
      return commit.amend(
        "HEAD",
        author,
        committer,
        messageEncoding,
        message,
        treeOid
      );
    })
    .then(function(commitId){
      amendedCommitId = commitId;
      return undoCommit();
    })
    .then(function(){
      assert.equal(amendedCommitId, expectedAmendedCommitId);
    });
  });

  it("can amend commit and update reference separately", function() {
    var customReflogMessage = "updating reference manually";

    var head, repo, oid, originalReflogCount;

    return NodeGit.Repository.open(reposPath)
    .then(function(repoResult) {
      repo = repoResult;
      // grab the original reflog entry count (to make sure .amend
      // doesn't add a reflog entry when not given a reference)
      return NodeGit.Reflog.read(repo, "HEAD");
    })
    .then(function(reflog) {
      originalReflogCount = reflog.entrycount();
      // get the head reference and commit
      return repo.head();
    })
    .then(function(headResult) {
      head = headResult;
      return repo.getHeadCommit();
    })
    .then(function(headCommit) {
      // amend the commit but don't update any reference
      // (passing null as update_ref)
      return headCommit.amend(
        null,
        null,
        null,
        "message",
        null,
        null);
    }).then(function(oidResult) {
      oid = oidResult;
      // update the reference manually
      return head.setTarget(oid, customReflogMessage);
    }).then(function() {
      // load reflog and make sure the last message is what we expected
      return NodeGit.Reflog.read(repo, "HEAD");
    }).then(function(reflog) {
      var reflogEntry = reflog.entryByIndex(0);
      assert.equal(
        reflogEntry.message(),
        customReflogMessage
      );
      assert.equal(
        reflogEntry.idNew().toString(),
        oid
      );
      // only setTarget should have added to the entrycount
      assert.equal(reflog.entrycount(), originalReflogCount + 1);
    });
  });

  it("has an owner", function() {
    var owner = this.commit.owner();
    assert.ok(owner instanceof Repository);
  });

  it("can walk its repository's history", function(done) {
    var historyCount = 0;
    var expectedHistoryCount = 364;

    var history = this.commit.history();

    history.on("commit", function(commit) {
      historyCount++;
    });

    history.on("end", function(commits) {
      assert.equal(historyCount, expectedHistoryCount);
      assert.equal(commits.length, expectedHistoryCount);

      done();
    });

    history.on("error", function(err) {
      assert.ok(false);
    });

    history.start();
  });

  it("can fetch the master branch HEAD", function() {
    var repository = this.repository;

    return repository.getBranchCommit("master").then(function(commit) {
      return repository.getCommit(commit.sha());
    });
  });

  it("can fetch all of its parents", function() {
    return this.commit.getParents().then(function(parents) {
      assert.equal(parents.length, 1);

      var sha = parents[0].sha();
      assert.equal(sha, "ecfd36c80a3e9081f200dfda2391acadb56dac27");
    });
  });

  it("can specify a parents limit", function() {
    return this.commit.getParents(0).then(function(parents) {
      assert.equal(parents.length, 0);
    });
  });

  it("can specify limit higher than actual parents", function() {
    return this.commit.getParents(2).then(function(parents) {
      assert.equal(parents.length, 1);
    });
  });

  it("can fetch parents of a merge commit", function () {
    return NodeGit.Repository.open(reposPath)
    .then(function (repo) {
      return repo.getCommit("bf1da765e357a9b936d6d511f2c7b78e0de53632");
    })
    .then(function (commit) {
      return commit.getParents();
    })
    .then(function (parents) {
      assert.equal(parents.length, 2);
    });
  });

  it("has a parent count", function() {
    assert.equal(1, this.commit.parentcount());
  });

  it("can retrieve and walk a commit tree", function() {
    var commitTreeEntryCount = 0;
    var expectedCommitTreeEntryCount = 198;

    return this.commit.getTree().then(function(tree) {
      return new Promise(function(resolve, fail) {

        var treeWalker = tree.walk();

        treeWalker.on("entry", function(entry) {
          commitTreeEntryCount++;
        });

        treeWalker.on("error", function(error) {
          fail(error);
        });

        treeWalker.on("end", function(entries) {
          try {
            assert.equal(commitTreeEntryCount, expectedCommitTreeEntryCount);
            resolve();
          }
          catch (e) {
            fail(e);
          }
        });

        treeWalker.start();
      });
    });
  });

  it("can get the commit diff", function() {
    return this.commit.getDiff().then(function(diff) {
      assert.equal(diff.length, 1);
    });
  });

  // it("can get the commit diff in large context", function() {
    // For displaying the full file we can set context_lines of options.
    // Eventually this should work, but right now there is a
    // comment in diff.c in libgit2 of "/* TODO: parse thresholds */"
    // It will add the "--unified" but not with the "=x" part.
    // options.context_lines = 20000;
  // });

  it("can get the commit diff without whitespace", function() {
    var repo;
    var options = {};
    var GIT_DIFF_IGNORE_WHITESPACE = (1 << 22);
    options.flags = GIT_DIFF_IGNORE_WHITESPACE;

    var fileName = "whitespacetest.txt";
    var fileContent = "line a\nline b\nline c\nline d\n	line e\nline f\n" +
                  "line g\nline h\nline i\n		line j\nline k\nline l\n" +
                  "line m\nline n\n			line o\nline p\nline q\n" +
                  "line r\nline s\nline t\nline u\nline v\nline w\n" +
                  "line x\nline y\nline z\n";
    var changedFileContent = "line a\nline b\n        line c\nline d\n" +
                  "line e\nline f\nline g\n  line h\nline i\nline j\n" +
                  "line k\nline l\nline m\nline n\nline o\nlinep\n" +
                  " line q\nline r\nline   s\nline t\n\nline u\n" +
                  "line v1\nline w\nline x\n			\nline y\nline z\n";

    return NodeGit.Repository.open(reposPath)
    .then(function(repoResult) {
      repo = repoResult;
      return commitFile(repo, fileName, fileContent, "commit this");
    })
    .then(function(){
      return commitFile(repo, fileName, changedFileContent, "commit that");
    })
    .then (function() {
      return repo.getHeadCommit();
    })
    .then (function(wsCommit) {
      return wsCommit.getDiffWithOptions(options);
    })
    .then(function(diff) {
      assert.equal(diff.length, 1);
      return diff[0].patches();
    })
    .then(function(patches) {
      assert.equal(patches.length, 1);
      var patch = patches[0];

      assert.equal(patch.oldFile().path(), fileName);
      assert.equal(patch.newFile().path(), fileName);
      assert.ok(patch.isModified());

      return patch.hunks();
    })
    .then(function(hunks) {
      return hunks[0].lines();
    })
    .then(function(lines) {
      //check all hunk lines
      assert.equal(lines.length, 12);
      assert.equal(lines[0].origin(), Diff.LINE.CONTEXT);

      assert.equal(lines[1].content().length, 9);
      assert.equal(lines[1].content(), "line   s\n");
      assert.equal(lines[1].origin(), Diff.LINE.CONTEXT);

      assert.equal(lines[2].origin(), Diff.LINE.CONTEXT);

      assert.equal(lines[3].content().length, 1);
      assert.equal(lines[3].content(), "\n");
      assert.equal(lines[3].origin(), Diff.LINE.ADDITION);

      assert.equal(lines[4].origin(), Diff.LINE.CONTEXT);

      assert.equal(lines[5].content().length, 7);
      assert.equal(lines[5].content(), "line v\n");
      assert.equal(lines[5].origin(), Diff.LINE.DELETION);

      assert.equal(lines[6].content().length, 8);
      assert.equal(lines[6].content(), "line v1\n");
      assert.equal(lines[6].origin(), Diff.LINE.ADDITION);

      assert.equal(lines[7].origin(), Diff.LINE.CONTEXT);

      assert.equal(lines[8].origin(), Diff.LINE.CONTEXT);

      assert.equal(lines[9].content().length, 4);
      assert.equal(lines[9].content(), "\t\t\t\n");
      assert.equal(lines[9].origin(), Diff.LINE.ADDITION);

      assert.equal(lines[10].origin(), Diff.LINE.CONTEXT);

      assert.equal(lines[11].origin(), Diff.LINE.CONTEXT);
    });
  });

  it("can get header fields", function() {
    var commit = this.commit;
    return commit.headerField("parent").then(function(field) {
      assert.equal(field,
        "ecfd36c80a3e9081f200dfda2391acadb56dac27");
      return commit.headerField("author");
    })
    .then(function(field) {
      assert.equal(field,
        "Michael Robinson <mike@panmedia.co.nz> 1362012884 +1300");
      return commit.headerField("committer");
    })
    .then(function(field) {
      assert.equal(field,
        "Michael Robinson <mike@panmedia.co.nz> 1362012884 +1300");
    });
  });

  it("can lookup using a short id", function () {
    return NodeGit.Repository.open(reposPath)
    .then(function (repo) {
      return Commit.lookupPrefix(repo, Oid.fromString("bf1da765"), 8);
    })
    .then(function (commit) {
      assert.equal(commit.id().toString(),
        "bf1da765e357a9b936d6d511f2c7b78e0de53632");
    });
  });

  it("can find nth gen ancestor", function () {
    return NodeGit.Repository.open(reposPath)
    .then(function (repo) {
      return repo.getCommit("b52067acaa755c3b3fc21b484ffed2bce4150f62");
    })
    .then(function (commit) {
      return commit.nthGenAncestor(3);
    })
    .then(function (commit) {
      assert.equal(commit.id().toString(),
        "9b2f3a37d46d47248d2704b6a46ec7e197bcd48c");
    });
  });

  describe("Commit's Author", function() {
    before(function() {
      this.author = this.commit.author();
    });

    it("is available", function() {
      assert.ok(this.author instanceof NodeGit.Signature);
    });

    it("has a name", function() {
      assert.equal(this.author.name(), "Michael Robinson");
    });

    it("has an email", function() {
      assert.equal(this.author.email(), "mike@panmedia.co.nz");
    });
  });

  describe("Commit's Committer", function() {
    before(function() {
      this.committer = this.commit.committer();
    });

    it("is available", function() {
      assert.ok(this.committer instanceof NodeGit.Signature);
    });

    it("has a name", function() {
      assert.equal(this.committer.name(), "Michael Robinson");
    });

    it("has an email", function() {
      assert.equal(this.committer.email(), "mike@panmedia.co.nz");
    });
  });

  describe("Commit's Body", function() {

    it("null if only summary", function() {
      var test = this;
      return NodeGit.Commit.lookup(test.repository,
        "15315cf41ad76400d9189c85a5827b77b8c392f1")
      .then(function(commit) {
        assert.equal(commit.body(), null);
      });
    });

    it("non-null when body exists", function() {
      var test = this;
      return NodeGit.Commit.lookup(test.repository,
        "c82fb078a192ea221c9f1093c64321c60d64aa0d")
      .then(function(commit) {
        assert.equal(commit.body(),
          "Added new methods in checkout and repository");
      });
    });
  });

  it("does not leak", function() {
    var test = this;

    return leakTest(NodeGit.Commit, function() {
        return NodeGit.Commit.lookup(test.repository, oid);
    });
  });

  it("duplicates signature", function() {
    garbageCollect();
    var Signature = NodeGit.Signature;
    var startSelfFreeingCount = Signature.getSelfFreeingInstanceCount();
    var startNonSelfFreeingCount =
      Signature.getNonSelfFreeingConstructedCount();
    var signature = this.commit.author();

    garbageCollect();
    var endSelfFreeingCount = Signature.getSelfFreeingInstanceCount();
    var endNonSelfFreeingCount = Signature.getNonSelfFreeingConstructedCount();
    // we should get one duplicated, self-freeing signature
    assert.equal(startSelfFreeingCount + 1, endSelfFreeingCount);
    assert.equal(startNonSelfFreeingCount, endNonSelfFreeingCount);

    signature = null;
    garbageCollect();
    endSelfFreeingCount = Signature.getSelfFreeingInstanceCount();
    // the self-freeing signature should get freed
    assert.equal(startSelfFreeingCount, endSelfFreeingCount);
  });

  describe("Commit's Signature", function() {
    it("Can retrieve the gpg signature from a commit", function() {
      var expectedSignedData =
        "tree f4661419a6fbbe865f78644fec722c023ce4b65f\n" +
        "parent 32789a79e71fbc9e04d3eff7425e1771eb595150\n" +
        "author Tyler Ang-Wanek <tylerw@axosoft.com> 1521227848 -0700\n" +
        "committer Tyler Ang-Wanek <tylerw@axosoft.com> 1521227848 -0700\n\n" +
        "GPG Signed commit\n";

      var expectedSignature =
        "-----BEGIN PGP SIGNATURE-----\n\n" +
        "iQEcBAABCAAGBQJarBhIAAoJEE8pfTd/81lKQA4IAL8Mu5kc4B/MX9s4XB26Ahap\n" +
        "n06kCx3RQ1KHMZIRomAjCnb48WieNVuy1y+Ut0RgfCxxrJ1ZnzFG3kF2bIKwIxNI\n" +
        "tYIC76iWny+mrVnb2mjKYjn/3F4c4VJGENq9ITiV1WeE4yJ8dHw2ox2D+hACzTvQ\n" +
        "KVroedk8BDFJxS6DFb20To35xbAVhwBnAGRcII4Wi5PPMFpqAhGLfq3Czv95ddSz\n" +
        "BHlyp27+YWSpV0Og0dqOEhsdDYaPrOBGRcoRiqjue+l5tgK/QerLFZ4aovZzpuEP\n" +
        "Xx1yZfqXIiy4Bo40qScSrdnmnp/kMq/NQGR3jYU+SleFHVKNFsya9UwurMaezY0=\n" +
        "=eZzi\n-----END PGP SIGNATURE-----";

      return NodeGit.Commit.lookup(
        this.repository,
        "cc1401eaac4e9e77190e98a9353b305f0c6313d8"
      )
        .then(function(commit) {
          return commit.getSignature();
        })
        .then(function(extractSignature) {
          assert.equal(
            extractSignature.signature,
            expectedSignature
          );
          assert.equal(
            extractSignature.signedData,
            expectedSignedData
          );
        });
    });
  });
});
