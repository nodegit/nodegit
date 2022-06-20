var assert = require("assert");
var path = require("path");
var local = path.join.bind(path, __dirname);
var fse = require("fs-extra");

describe("Tree", function() {
  var NodeGit = require("../../");
  var RepoUtils = require("../utils/repository_setup");

  var repoPath = local("../repos/tree");
  var existingPath = local("../repos/workdir");
  var oid = "5716e9757886eaf38d51c86b192258c960d9cfea";

  beforeEach(function() {
    var test = this;
    return RepoUtils.createRepository(repoPath)
      .then(function(repo) {
        test.repository = repo;
      }).then(function() {
        return NodeGit.Repository.open(existingPath);
      }).then(function(repository) {
        test.existingRepo = repository;
        return repository.getCommit(oid);
      }).then(function(commit) {
        test.commit = commit;
      });
  });

  after(function() {
    return fse.remove(repoPath);
  });

  it("gets an entry by name",
  function() {
    return this.commit.getTree().then(function(tree) {
      var entry = tree.entryByName("README.md");
        assert(entry);
    });
  });

  it("updates a tree", function () {
    var repo = this.existingRepo;
    var update = new NodeGit.TreeUpdate();
    update.action = NodeGit.Tree.UPDATE.REMOVE;
    update.path = "README.md";
    return this.commit.getTree().then(function(tree) {
        return tree.createUpdated(repo, 1, [update]);
      })
      .then(function(treeOid) {
        return repo.getTree(treeOid);
      })
      .then(function(updatedTree) {
        assert.throws(function () {
          updatedTree.entryByName("README.md");
        });
      });
  });

  it("walks its entries and returns the same entries on both progress and end",
  function() {
    var repo = this.repository;
    var file1 = "test.txt";
    var file2 = "foo/bar.txt";
    var expectedPaths = [file1, file2];
    var progressEntries = [];
    var endEntries;

    return RepoUtils.commitFileToRepo(repo, file1, "")
      .then(function(commit) {
        return RepoUtils.commitFileToRepo(repo, file2, "", commit);
      })
      .then(function(commit) {
        return commit.getTree();
      })
      .then(function(tree) {
        assert(tree);

        return new Promise(function (resolve, reject) {
          var walker = tree.walk();

          walker.on("entry", function(entry) {
            progressEntries.push(entry);
          });
          walker.on("end", function(entries) {
            endEntries = entries;
            resolve();
          });
          walker.on("error", reject);

          walker.start();
        });
      })
      .then(function() {
        assert(progressEntries.length);
        assert(endEntries && endEntries.length);

        function getEntryPath(entry) {
          return entry.path();
        }

        var progressFilePaths = progressEntries.map(getEntryPath);
        var endFilePaths = endEntries.map(getEntryPath);

        assert.deepEqual(
          expectedPaths, progressFilePaths,
          "progress entry paths do not match expected paths"
        );

        assert.deepEqual(
          expectedPaths, endFilePaths,
          "end entry paths do not match expected paths"
        );
      });
  });

  it("get all paths from a tree", async function () {
    const tree = await this.commit.getTree();
    const paths = await tree.getAllFilepaths();
    assert.equal(paths.length, 512);
    assert.equal(paths[0], ".gitignore");
    assert.equal(paths[511], "wscript");
  });

});
