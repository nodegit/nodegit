var git = require('../');
var path = require('path');
var Promise = require('nodegit-promise');
var promisify = require('promisify-node');
var fse = promisify(require('fs-extra'));
var fileName = 'newfile.txt';
var fileContent = 'hello world';

/**
 * This example creates a certain file `newfile.txt`, adds it to the git index and
 * commits it to head. Similar to a `git add newfile.txt` followed by a `git commit`
**/

var repo;
var index;
var oid;
var parent;

git.Repository.open(path.resolve(__dirname, '../.git'))
.then(function(repoResult) {
  repo = repoResult;
  return fse.writeFile(path.join(repo.workdir(), fileName), fileContent);
})
.then(function() {
  return repo.openIndex();
})
.then(function(indexResult) {
  index = indexResult;
  return index.read(1);
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
  oid = oidResult;
  return git.Refs.nameToId(repo, 'HEAD');
})
.then(function(head) {
  return repo.getCommit(head);
})
.then(function(parent) {
  var author = git.Signature.create("Scott Chacon", "schacon@gmail.com", 123456789, 60);
  var committer = git.Signature.create("Scott A Chacon", "scott@github.com", 987654321, 90);

  return repo.createCommit('HEAD', author, committer, 'message', oid, [parent]);
})
.done(function(commitId) {
  console.log('New Commit: ', commitId);
});
