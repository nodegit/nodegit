var nodegit = require('../');
var path = require('path');
var Promise = require('nodegit-promise');
var promisify = require('promisify-node');
var fse = promisify(require('fs-extra'));
fse.ensureDir = promisify(fse.ensureDir);

var repoDir = '../../newRepo';
var fileName = 'newFile.txt';

var baseFileContent = 'All Bobs are created equal. ish.';
var ourFileContent = "Big Bobs are best, IMHO.";
var theirFileContent = "Nobody expects the small Bobquisition!";
var finalFileContent = "Big Bobs are beautiful, and the small are unexpected!";

var baseSignature = nodegit.Signature.create("Peaceful Bob", "justchill@bob.net", 123456789, 60);
var ourSignature = nodegit.Signature.create("Big Bob", "impressive@bob.net", 123456789, 60);
var theirSignature = nodegit.Signature.create("Small Bob", "underestimated@bob.net", 123456789, 60);

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
  return fse.writeFile(path.join(repository.workdir(), fileName), baseFileContent);
})


// Load up the repository index and make our initial commit to HEAD
.then(function() {
  return repository.openIndex();
})
.then(function(index) {
  index.read(1);
  index.addByPath(fileName);
  index.write()

  return index.writeTree();
})
.then(function(oid) {
  return repository.createCommit('HEAD', baseSignature, baseSignature, 'bobs are all ok', oid, []);
})
.then(function(commitOid) {
  baseCommitOid = commitOid;
  return repository.getCommit(commitOid).then(function(commit) {
    baseCommit = commit;
  });
})


// create our branches
.then(function() {
  return repository.createBranch(ourBranchName, baseCommitOid).then(function(branch) {
    ourBranch = branch;
  });
})
.then(function() {
  return repository.createBranch(theirBranchName, baseCommitOid).then(function(branch) {
    theirBranch = branch;
  });
})


// Write and commit our version of the file
.then(function() {
  return fse.writeFile(path.join(repository.workdir(), fileName), ourFileContent);
})
.then(function() {
  return repository.openIndex().then(function(index) {
    index.read(1);
    index.addByPath(fileName);
    index.write()

    console.log('OURS');
    for (var i = 0, l = index.entryCount(); i < l; i++) {
      var entry = index.getByIndex(i);
      console.log(entry.id().toString() + " " + entry.path());
    }
    console.log('\n\n');

    return index.writeTree();
  });
})
.then(function(oid) {
  return repository.createCommit(ourBranch.name(), ourSignature, ourSignature, 'lol big bobs :yesway:', oid, [baseCommit]);
})
.then(function(commitOid) {
  return repository.getCommit(commitOid).then(function(commit) {
    ourCommit = commit;
  });
})


// Write and commit their version of the file
.then(function() {
  return fse.writeFile(path.join(repository.workdir(), fileName), theirFileContent);
})
.then(function() {
  return repository.openIndex().then(function(index) {
    index.read(1);
    index.addByPath(fileName);
    index.write()
    console.log('Theirs');
    for (var i = 0, l = index.entryCount(); i < l; i++) {
      var entry = index.getByIndex(i);
      console.log(entry.id().toString() + " " + entry.path());
    }
    console.log('\n\n');

    return index.writeTree();
  });
})
.then(function(oid) {
  return repository.createCommit(theirBranch.name(), theirSignature, theirSignature, 'lol big bobs :poop:', oid, [baseCommit]);
})
.then(function(commitOid) {
  return repository.getCommit(commitOid).then(function(commit) {
    theirCommit = commit;
  });
})

// Merge their branch into our branch
.then(function() {
  return nodegit.Merge.commits(repository, ourCommit, theirCommit, null);
})

// Merging returns an index that isn't backed by the repository.
// You have to write it to the repository instead of just writing it.
.then(function(index) {
  return nodegit.Reference.lookup(repository, 'HEAD').then(function(head) {
    return head.symbolicSetTarget(ourBranch.name(), ourSignature, "");
  }).then(function() {
    if (index.hasConflicts) {
      console.log('Conflict time!');
      for (var i = 0, l = index.entryCount(); i < l; i++) {
        var entry = index.getByIndex(i);
        console.log(entry.id().toString() + " " + entry.path());
      }
      console.log('\n\n');
      //if the merge had comflicts, solve them (in this case, we simply overwrite the file)
      fse.writeFileSync(path.join(repository.workdir(), fileName), finalFileContent);
      // read the fs updates into the index
      index.read(1);

      index.addByPath(fileName);
      // mark all conflicts as resolved
      index.conflictCleanup();
      // write back to the index
      index.write();
    }

    return index.writeTreeTo(repository);
  });
})
.then(function(oid) {
  // create the new merge commit on our branch
  return repository.createCommit(ourBranch.name(), baseSignature, baseSignature, 'Stop this bob sized fued', oid, [ourCommit, theirCommit]);
})
.done(function(commitId) {
  console.log('New Commit: ', commitId);
});
