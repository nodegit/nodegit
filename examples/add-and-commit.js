var nodegit = require("../");
var path = require("path");
var fse = require("fs-extra");
var fileName = "newfile.txt";
var fileContent = "hello world";
var directoryName = "salad/toast/strangerinastrangeland/theresnowaythisexists";

/**
 * This example creates a certain file `newfile.txt`, adds it to the git
 * index and commits it to head. Similar to a `git add newfile.txt`
 * followed by a `git commit`
**/

var repo;
var index;
var oid;

nodegit.Repository.open(path.resolve(__dirname, "../.git"))
.then(function(repoResult) {
  repo = repoResult;
  return fse.ensureDir(path.join(repo.workdir(), directoryName));
}).then(function(){
  return fse.writeFile(path.join(repo.workdir(), fileName), fileContent);
})
.then(function() {
  return fse.writeFile(
    path.join(repo.workdir(), directoryName, fileName),
    fileContent
  );
})
.then(function() {
  return repo.refreshIndex();
})
.then(function(indexResult) {
  index = indexResult;
})
.then(function() {
  // this file is in the root of the directory and doesn't need a full path
  return index.addByPath(fileName);
})
.then(function() {
  // this file is in a subdirectory and can use a relative path
  return index.addByPath(path.posix.join(directoryName, fileName));
})
.then(function() {
  // this will write both files to the index
  return index.write();
})
.then(function() {
  return index.writeTree();
})
.then(function(oidResult) {
  oid = oidResult;
  return nodegit.Reference.nameToId(repo, "HEAD");
})
.then(function(head) {
  return repo.getCommit(head);
})
.then(function(parent) {
  var author = nodegit.Signature.create("Scott Chacon",
    "schacon@gmail.com", 123456789, 60);
  var committer = nodegit.Signature.create("Scott A Chacon",
    "scott@github.com", 987654321, 90);

  return repo.createCommit("HEAD", author, committer, "message", oid, [parent]);
})
.done(function(commitId) {
  console.log("New Commit: ", commitId);
});
