var NodeGit = require("../../");
var path = require("path");
var fse = require("fs-extra");
var RepoUtils = require("../utils/repository_setup");

var IndexSetup = {
  createConflict: function createConflict(
    repository,
    _ourBranchName,
    _theirBranchName,
    _fileName
  ) {
    var fileName = _fileName || "everyonesFile.txt";

    var ourBranchName = _ourBranchName || "ours";
    var theirBranchName = _theirBranchName || "theirs";

    var baseFileContent = "How do you feel about Toll Roads?\n";
    var ourFileContent = "I like Toll Roads. I have an EZ-Pass!\n";
    var theirFileContent = "I'm skeptical about Toll Roads\n";

    var ourSignature = NodeGit.Signature.create
          ("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
    var theirSignature = NodeGit.Signature.create
          ("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

    var ourCommit;
    var ourBranch;
    var theirBranch;

    return fse.writeFile(
      path.join(repository.workdir(), fileName),
      baseFileContent
    )
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        return repository.createCommit("HEAD", ourSignature,
          ourSignature, "initial commit", oid, []);
      })
      .then(function(commitOid) {
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
        return fse.writeFile(path.join(repository.workdir(), fileName),
          baseFileContent + theirFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        return repository.createCommit(theirBranch.name(), theirSignature,
          theirSignature, "they made a commit", oid, [ourCommit]);
      })
      .then(function(commitOid) {
        return fse.writeFile(path.join(repository.workdir(), fileName),
          baseFileContent + ourFileContent);
      })
      .then(function() {
        return RepoUtils.addFileToIndex(repository, fileName);
      })
      .then(function(oid) {
        return repository.createCommit(ourBranch.name(), ourSignature,
            ourSignature, "we made a commit", oid, [ourCommit]);
      })
      .then(function() {
        return repository.checkoutBranch(
          ourBranch,
          new NodeGit.CheckoutOptions()
        );
      })
      .then(function() {
        return repository.mergeBranches(ourBranchName, theirBranchName);
      })
      .catch(function(index) {
        return NodeGit.Checkout.index(repository, index)
          .then(function() { return index; });
      });
  }
};

module.exports = IndexSetup;
