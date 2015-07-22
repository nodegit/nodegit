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
    return repo.openIndex();
  })
  .then(function(index){
    _index = index;
    return _index.read();
  })
  .then(function() {
    //remove the file from the index...
    _index.removeByPath(fileName);
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
    var author = nodegit.Signature.create("Scott Chacon",
      "schacon@gmail.com", 123456789, 60);
    var committer = nodegit.Signature.create("Scott A Chacon",
      "scott@github.com", 987654321, 90);

    return _repository.createCommit("HEAD", author, committer,
      "message", _oid, [parent]);
  })
  .then(function(commitId) {
    // the file is removed from the git repo, use fs.unlink now to remove it
    // from the filesystem.
    console.log("New Commit:", commitId.allocfmt());
  })
  .done();
