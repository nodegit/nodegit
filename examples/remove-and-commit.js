var nodegit = require("../"),
    path = require("path"),
    fileName = "newfile.txt";

/**
 * This example deletes a certain file `newfile.txt`,
 * removes it from the git index and commits it to head. Similar to a
 * `git rm newfile.txt` followed by a `git commit`. Use add-and-commit.js
 * to create the file first.
**/

var _repository;
var _index;
var _oid;

//open a git repo
nodegit.Repository.open(path.resolve(__dirname, "../.git"))
  .then(function(repo) {
    _repository = repo;
    return repo.refreshIndex();
  })
  .then(function(index){
    _index = index;
  })
  .then(function() {
    //remove the file from the index...
    return _index.removeByPath(fileName);
  })
  .then(function() {
    return _index.write();
  })
  .then(function() {
    return _index.writeTree();
  })
  .then(function(oid) {
    _oid = oid;
    return nodegit.Reference.nameToId(_repository, "HEAD");
  })
  .then(function(head) {
    return _repository.getCommit(head);
  })
  .then(function(parent) {
    var author = nodegit.Signature.now("Scott Chacon",
      "schacon@gmail.com");
    var committer = nodegit.Signature.now("Scott A Chacon",
      "scott@github.com");

    return _repository.createCommit("HEAD", author, committer,
      "message", _oid, [parent]);
  })
  .then(function(commitId) {
    // the file is removed from the git repo, use fs.unlink now to remove it
    // from the filesystem.
    console.log("New Commit:", commitId.allocfmt());
  })
  .done();
