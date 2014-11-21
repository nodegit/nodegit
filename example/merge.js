var nodegit = require('../');
var path = require('path');
var Promise = require('nodegit-promise');
var promisify = require('promisify-node');
var fse = promisify(require('fs-extra'));
fse.ensureDir = promisify(fse.ensureDir);

var normalizeOptions = require('../lib/util/normalize_options');
var ourFileName = 'ourNewFile.txt';
var ourFileContent = 'I like Toll Roads. I have an EZ-Pass!';
var ourBranchName = "ours";

var theirFileName = 'theirNewFile.txt';
var theirFileContent = "I'm skeptical about Toll Roads";
var theirBranchName = "theirs";

var repoDir = '../../newRepo';

var repository;
var ourCommit;
var theirCommit;
var ourBranch;
var theirBranch;

var ourSignature = nodegit.Signature.create("Ron Paul", "RonPaul@TollRoadsRBest.info", 123456789, 60);
var theirSignature = nodegit.Signature.create("Greg Abbott", "Gregggg@IllTollYourFace.us", 123456789, 60);

fse.remove(path.resolve(__dirname, repoDir))
.then(function() {
  return fse.ensureDir(path.resolve(__dirname, repoDir));
})
.then(function() {
  return nodegit.Repository.init(path.resolve(__dirname, repoDir), 0);
})
.then(function(repo) {
  repository = repo;
  return fse.writeFile(path.join(repository.workdir(), ourFileName), ourFileContent);
})
.then(function() {
  return repository.openIndex();
})
.then(function(index) {
  index.read(1);
  index.addByPath(ourFileName);
  index.write()

  return index.writeTree();
})
.then(function(oid) {
  return repository.createCommit('HEAD', ourSignature, ourSignature, 'we made a commit', oid, []);
}).then(function(commitOid) {

  return repository.getCommit(commitOid).then(function(commit) {
    ourCommit = commit;
  }).then(function() {
    return repository.createBranch(ourBranchName, commitOid).then(function(branch) {
      ourBranch = branch;
      return repository.createBranch(theirBranchName, commitOid);
    });
  });
}).then(function(branch) {
  theirBranch = branch;

  return nodegit.Reference.lookup(repository, 'HEAD').then(function(head) {
    return head.symbolicSetTarget(theirBranch.name(), theirSignature, "");
  });
}).then(function() {
  return fse.writeFile(path.join(repository.workdir(), theirFileName), theirFileContent);
})
.then(function() {
  return repository.openIndex();
})
.then(function(index) {
  index.read(1);
  index.addByPath(theirFileName);
  index.write()

  return index.writeTree();
})
.then(function(oid) {
  return repository.createCommit('HEAD', theirSignature, theirSignature, 'they made a commit', oid, [ourCommit]);
})
.then(function(commitOid) {
  return repository.getCommit(commitOid).then(function(commit) {
    theirCommit = commit;
  }).then(function() {
    return nodegit.Reference.lookup(repository, 'HEAD').then(function(head) {
      return head.symbolicSetTarget(ourBranch.name(), ourSignature, "");
    });
  });
})
.then(function() {
  return nodegit.Merge.commits(repository, ourCommit, theirCommit, new nodegit.MergeOptions());
})
.then(function(index) {
  index.write()
  return index.writeTreeTo(repository);
})
.then(function(oid) {
  return repository.createCommit(ourBranch.name(), ourSignature, ourSignature, 'we merged their commit', oid, [ourCommit, theirCommit]);
})
.done(function(commitId) {
  console.log('New Commit: ', commitId);
});
