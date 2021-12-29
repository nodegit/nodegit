var nodegit = require("../");
var path = require("path");
var fse = require("fs-extra");

var repoDir = "../../newRepo";
var fileName = "newFile.txt";

var baseFileContent = "All Bobs are created equal. ish.\n";
var ourFileContent = "Big Bobs are best, IMHO.\n";
var theirFileContent = "Nobody expects the small Bobquisition!\n";
var finalFileContent = "Big Bobs are beautiful and the small are unexpected!\n";

var baseSignature = nodegit.Signature.now("Peaceful Bob",
  "justchill@bob.net");
var ourSignature = nodegit.Signature.now("Big Bob",
  "impressive@bob.net");
var theirSignature = nodegit.Signature.now("Small Bob",
  "underestimated@bob.net");

var ourBranchName = "ours";
var theirBranchName = "theirs";

var repository;
var baseCommit;
var baseCommitOid;
var ourCommit;
var theirCommit;
var ourBranch;
var theirBranch;

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
    path.join(repository.workdir(), fileName),
    baseFileContent
  );
})


// Load up the repository index and make our initial commit to HEAD
.then(function() {
  return repository.refreshIndex();
})
.then(function(index) {
  return index.addByPath(fileName)
    .then(function() {
      return index.write();
    })
    .then(function() {
      return index.writeTree();
    });
})
.then(function(oid) {
  return repository.createCommit("HEAD", baseSignature,
    baseSignature, "bobs are all ok", oid, []);
})
.then(function(commitOid) {
  baseCommitOid = commitOid;
  return repository.getCommit(commitOid)
    .then(function(commit) {
      baseCommit = commit;
    });
})


// create our branches
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


// Write and commit our version of the file
.then(function() {
  return fse.writeFile(
    path.join(repository.workdir(), fileName),
    ourFileContent
  );
})
.then(function() {
  return repository.refreshIndex()
    .then(function(index) {
      return index.addByPath(fileName)
        .then(function() {
          return index.write();
        })
        .then(function() {
          return index.writeTree();
        });
    });
})
.then(function(oid) {
  return repository.createCommit(ourBranch.name(), ourSignature,
    ourSignature, "lol big bobs :yesway:", oid, [baseCommit]);
})
.then(function(commitOid) {
  return repository.getCommit(commitOid).then(function(commit) {
    ourCommit = commit;
  });
})


// Write and commit their version of the file
.then(function() {
  return fse.writeFile(
    path.join(repository.workdir(), fileName),
    theirFileContent
  );
})
.then(function() {
  return repository.refreshIndex()
    .then(function(index) {
      return index.addByPath(fileName)
        .then(function() {
          return index.write();
        })
        .then(function() {
          return index.writeTree();
        });
  });
})
.then(function(oid) {
  return repository.createCommit(theirBranch.name(), theirSignature,
    theirSignature, "lol big bobs :poop:", oid, [baseCommit]);
})
.then(function(commitOid) {
  return repository.getCommit(commitOid).then(function(commit) {
    theirCommit = commit;
  });
})


// move the head to our branch, just to keep things tidy
.then(function() {
  return nodegit.Reference.lookup(repository, "HEAD");
})
.then(function(head) {
  return head.symbolicSetTarget(ourBranch.name(), "");
})


// Merge their branch into our branch
.then(function() {
  return nodegit.Merge.commits(repository, ourCommit, theirCommit, null);
})

// Merging returns an index that isn't backed by the repository.
// You have to write it to the repository instead of just writing it.
.then(function(index) {
  if (index.hasConflicts()) {
    console.log("Conflict time!");

    // if the merge had comflicts, solve them
    // (in this case, we simply overwrite the file)
    fse.writeFileSync(
      path.join(repository.workdir(), fileName),
      finalFileContent
    );
  }
})

// we need to get a new index as the other one is not backed to
// the repository in the usual fashion, and just behaves weirdly.
.then(function() {
  return repository.refreshIndex()
    .then(function(index) {
      return index.addByPath(fileName)
        .then(function() {
          return index.write();
        })
        .then(function() {
          return index.writeTree();
        });
  });
})
.then(function(oid) {
  // create the new merge commit on our branch
  return repository.createCommit(ourBranch.name(), baseSignature,
    baseSignature, "Stop this bob sized fued", oid, [ourCommit, theirCommit]);
})
.done(function(commitId) {
  console.log("New Commit: ", commitId);
});
