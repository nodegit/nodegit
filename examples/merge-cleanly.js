var nodegit = require("../");
var path = require("path");
var fse = require("fs-extra");

var ourFileName = "ourNewFile.txt";
var ourFileContent = "I like Toll Roads. I have an EZ-Pass!";
var ourBranchName = "ours";

var theirFileName = "theirNewFile.txt";
var theirFileContent = "I'm skeptical about Toll Roads";
var theirBranchName = "theirs";

var repoDir = "../../newRepo";

var repository;
var ourCommit;
var theirCommit;
var ourBranch;
var theirBranch;

var ourSignature = nodegit.Signature.now("Ron Paul",
  "RonPaul@TollRoadsRBest.info");
var theirSignature = nodegit.Signature.now("Greg Abbott",
  "Gregggg@IllTollYourFace.us");

// Create a new repository in a clean directory, and add our first file
fse.remove(path.resolve(__dirname, repoDir))
.then(function() {
  return fse.ensureDir(path.resolve(__dirname, repoDir));
})
.then(function() {
  return nodegit.Repository.init(path.resolve(__dirname, repoDir), 0);
})
.then(function(repo) {
  repository = repo;
  return fse.writeFile(
    path.join(repository.workdir(), ourFileName),
    ourFileContent
  );
})

// Load up the repository index and make our initial commit to HEAD
.then(function() {
  return repository.refreshIndex();
})
.then(function(index) {
  return index.addByPath(ourFileName)
    .then(function() {
      return index.write();
    })
    .then(function() {
      return index.writeTree();
    });
})
.then(function(oid) {
  return repository.createCommit("HEAD", ourSignature,
    ourSignature, "we made a commit", oid, []);
})

// Get commit object from the oid, and create our new branches at that position
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

// Create a new file, stage it and commit it to our second branch
.then(function(branch) {
  theirBranch = branch;
  return fse.writeFile(
    path.join(repository.workdir(), theirFileName),
    theirFileContent
  );
})
.then(function() {
  return repository.refreshIndex();
})
.then(function(index) {
  return index.addByPath(theirFileName)
    .then(function() {
      return index.write();
    })
    .then(function() {
      return index.writeTree();
    });
})
.then(function(oid) {
  // You don"t have to change head to make a commit to a different branch.
  return repository.createCommit(theirBranch.name(), theirSignature,
    theirSignature, "they made a commit", oid, [ourCommit]);
})
.then(function(commitOid) {
  return repository.getCommit(commitOid).then(function(commit) {
    theirCommit = commit;
  });
})


// Merge the two commits
.then(function() {
  return nodegit.Merge.commits(repository, ourCommit, theirCommit);
})


// Merging returns an index that isn't backed by the repository.
// You have to manually check for merge conflicts. If there are none
// you just have to write the index. You do have to write it to
// the repository instead of just writing it.
.then(function(index) {
  if (!index.hasConflicts()) {
    return index.writeTreeTo(repository);
  }
})


// Create our merge commit back on our branch
.then(function(oid) {
  return repository.createCommit(ourBranch.name(), ourSignature,
    ourSignature, "we merged their commit", oid, [ourCommit, theirCommit]);
})
.done(function(commitId) {
  // We never changed the HEAD after the initial commit;
  // it should still be the same as master.
  console.log("New Commit: ", commitId);
});
