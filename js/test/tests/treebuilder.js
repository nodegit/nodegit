var assert = require("assert");
var path = require("path");
var fs = require("fs");
var promisify = require("promisify-node");
var readDir = promisify(fs.readdir);
var local = path.join.bind(path, __dirname);

describe("TreeBuilder", function(){

    var Git = require("../../");
    var reposPath = local("../repos/workdir");
    //setup test repo each test
    beforeEach(function() {
      var test = this;

      return Git.Repository.open(reposPath)
        .then(function(repo) {
          test.repo = repo;
        });
    });
    //treebuilder created with no source when creating a new folder
    //  (each folder in git is a tree)
    //  or the root folder for a root commit
    it("Can create a new treebuilder with no source", function(){

        return Git.Treebuilder.create(this.repo, null);
    });
    //treebuilder created with a source tree can add / read from tree
    it("Can create a treebuilder from the latest commit tree", function(){

      var test = this;
      //get latest commit
      return test.repo.getHeadCommit()
      //get tree of commit
      .then(function(commit){ return commit.getTree(); })
      //make treebuilder from tree
      .then(function(tree){ return Git.Treebuilder.create(test.repo, tree); })
      //verify treebuilder can do stuff
      .then(function(treeBuilder){
        //check
        //count how many entries we should have
        return readDir(reposPath)
        //treebuilder should have all entries in the clean working dir
        //(minus .git folder)
        .then(function(dirEntries) {
          return assert.equal(dirEntries.length-1, treeBuilder.entrycount());
        });
      });
    });
    //adding a tree is adding a folder
    it("Can add a new tree to an existing tree", function(){

      var test = this;
      //get latest commit
      return test.repo.getHeadCommit()
      //get tree of commit
      .then(function(commit){ return commit.getTree(); })
      //make treebuilder from tree
      .then(function(tree){ return Git.Treebuilder.create(test.repo, tree); })
      //verify treebuilder can do stuff
      .then(function(rootTreeBuilder){
        //new dir builder
        return Git.Treebuilder.create(test.repo, null)
        .then(function(newTreeBuilder){
          //insert new dir
          return rootTreeBuilder.insert(
            "mynewfolder",
            newTreeBuilder.write(),
            Git.TreeEntry.FILEMODE.TREE
          );
        });
      })
      .then(function(newTreeEntry){
        assert(newTreeEntry.isTree(),
          "Created a tree (new folder) that is a tree");
        return Git.Tree.lookup(test.repo, newTreeEntry.oid());
      });
    });
});
